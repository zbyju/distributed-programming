#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

unsigned long recursionCount;
unsigned int maxWeight;
enum EdgeState { s_undefined, included, excluded };
enum NodeColor { c_undefined, red, green };
using Edge = tuple<uint8_t, uint8_t, unsigned int, EdgeState>;

/**
 * @brief Finds an edge in the vector based on id's. It ignores order of id's
 * (symmetrical find).
 *
 * @param edges - vector of edges
 * @param id1 - id of one of the nodes
 * @param id2 - id of the other node
 * @return unique_ptr<Edge> - reference to the edge
 */
unique_ptr<Edge> findEdgeByIds(vector<Edge> edges, uint8_t id1, uint8_t id2) {
  for (auto &e : edges) {
    if (get<0>(e) == id1 && get<1>(e) == id2) return make_unique<Edge>(e);
    if (get<0>(e) == id2 && get<1>(e) == id1) return make_unique<Edge>(e);
  }
  return nullptr;
}

/**
 * @brief Prints adjacency matrix.
 *
 * @param n - number of nodes
 * @param edges - vector of edges with weights
 */
void printGraph(unsigned int n, vector<Edge> &edges) {
  string res = "";
  unsigned int maxWeight = 0;
  for (auto &e : edges) {
    if (get<2>(e) > maxWeight) maxWeight = get<2>(e);
  }
  unsigned int maxLength = to_string(maxWeight).length();
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      auto edge = findEdgeByIds(edges, i, j);
      string weight = "0";
      if (edge != nullptr) weight = to_string(get<2>(*edge));
      string padding = " ";
      while (padding.length() + weight.length() < (unsigned)maxLength + 1) {
        padding += " ";
      }
      res += weight + padding;
    }
    res += "\n";
  }
  cout << res << endl;
}

/**
 * @brief Prints weights of all the edges in the order that they are stored in.
 *
 * @param edges - vector of edges to be printed
 */
void printEdgeWeights(vector<Edge> &edges) {
  for (auto &e : edges) {
    cout << get<2>(e) << endl;
  }
}

/**
 * @brief Prints weight, time and # of recursion calls.
 *
 * @param time
 */
void printResult(double time) {
  cout << endl << "=========================" << endl;
  cout << "Max weight: " << maxWeight << endl;
  cout << "Calculation time: " << time << endl;
  cout << "Recursion call count: " << recursionCount << endl;
}

/**
 * @brief Parses args from terminal.
 *
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return string - returns file path to the input file
 */
string parseArgs(int argc, char *argv[]) {
  if (argc != 3) return "";
  if (strcmp(argv[1], "-f") != 0) return "";
  return argv[2];
}

/**
 * @brief Goes through an input file and returns number of nodes.
 * It also adds all the edges (structure of the graph) into the provided vector.
 *
 * @param inputPath - string representing the path to the input file
 * @param edges - vector for adding all the edges to
 * @return unsigned int n - returns number of nodes
 */
unsigned int parseFile(string &inputPath, vector<Edge> &edges) {
  ifstream infile(inputPath);
  string line;
  string firstLine;
  // Get N from the first line
  getline(infile, firstLine);
  uint8_t n = stoul(firstLine);

  // Go through all the weights and add them to the graph
  uint8_t i = 0;
  uint8_t j = 0;
  while (getline(infile, line)) {
    istringstream buffer(line);
    for (string weightString; buffer >> weightString;) {
      uint32_t weight = stoul(weightString);
      if (weight != 0 && findEdgeByIds(edges, i, j) == nullptr) {
        edges.push_back(make_tuple(i, j, weight, s_undefined));
      }
      ++j;
    }
    j = 0;
    ++i;
  }
  return n;
}

/**
 * @brief Takes two timestamps and returns time difference in seconds.
 *
 * @param start - start time
 * @param end - end time
 * @return double - time in seconds
 */
double calculateTime(clock_t &start, clock_t &end) {
  return double(end - start) / CLOCKS_PER_SEC;
}

/**
 * @brief Function for sorting edges by weight (highest weight first).
 */
bool sortByWeight(const Edge &e1, const Edge &e2) {
  return get<2>(e1) > get<2>(e2);
}

/**
 * @brief Sums up all the weights of edges that were included in the subgraph.
 *
 * @param edges - vector of edges
 * @return unsigned int - total weight of all included edges
 */
unsigned int getChosenWeight(const vector<Edge> &edges) {
  unsigned int sum = 0;
  for (auto &e : edges) {
    if (get<3>(e) == included) sum += get<2>(e);
  }
  return sum;
}

/**
 * @brief Sums up all the weights of edges that were not excluded from the
 * subgraph.
 *
 * @param edges - vector of edges
 * @return unsigned int - total weight of all included/undefined edges.
 */
unsigned int getPotentialWeight(const vector<Edge> &edges) {
  unsigned int sum = 0;
  for (auto &e : edges) {
    if (get<3>(e) != excluded) sum += get<2>(e);
  }
  return sum;
}

/**
 * @brief Checks if the edge can be colored Red - Green or if there would be a
 * conflict.
 */
bool canColorRedGreen(const vector<NodeColor> &colors, const Edge &edge) {
  return colors[get<0>(edge)] != green && colors[get<1>(edge)] != red;
}

/**
 * @brief Checks if the edge can be colored Green - Red or if there would be a
 * conflict.
 */
bool canColorGreenRed(const vector<NodeColor> &colors, const Edge &edge) {
  return colors[get<0>(edge)] != red && colors[get<1>(edge)] != green;
}

/**
 * @brief Checks if it makes sense to not include this edge - if both nodes have
 * a non-matching color then not including this edge would be strictly worse
 * than including it.
 */
bool shouldTryNotAdding(const vector<NodeColor> &colors, const Edge &edge) {
  NodeColor c1 = colors[get<0>(edge)];
  NodeColor c2 = colors[get<1>(edge)];
  return !((c1 == red && c2 == green) || (c1 == green && c2 == red));
}

/**
 * @brief Checks if the included edges create a connected subgraph.
 *
 * @param n - number of nodes
 * @param edges - vector of edges
 * @return bool - True if connected, otherwise false
 */
bool isConnected(unsigned int n, vector<Edge> &edges) {
  vector<bool> connected(n, false);
  unsigned int m = 0;
  for (const auto &e : edges) {
    if (get<3>(e) != included) continue;

    unsigned int id1 = get<0>(e);
    unsigned int id2 = get<1>(e);
    if (connected[id1] == false) {
      ++m;
      connected[id1] = true;
    }
    if (connected[id2] == false) {
      ++m;
      connected[id2] = true;
    }
    if (m >= n) return true;
  }
  return m >= n;
}

/**
 * @brief The main recursive function that checks all the possible subgraphs and
 * finds the max weight.
 *
 * @param colors - vector of colors of the nodes (i-th position = color of the
 * i'th node)
 * @param edges - vector of edges sorted from the heaviest to the lightest.
 * @param index - index of currently checked edge
 * @param chosenWeight - total weight of all included edges (this is for
 * optimization)
 * @param potentialWeight - total potential weight of all edges (this is for
 * optimization)
 */
void solveDFS(vector<NodeColor> &colors, vector<Edge> &edges,
              unsigned int index, unsigned int chosenWeight,
              unsigned int potentialWeight) {
  ++recursionCount;

  // Check if it is connected and overwrite best result if current is better
  // (the graph is bipartite, no need to check that)
  if (chosenWeight > maxWeight && isConnected(colors.size(), edges))
    maxWeight = chosenWeight;

  // Check if we are at the end of the calculation or if there is still a
  // potential to beat the best score (weight)
  if (potentialWeight <= maxWeight || index >= edges.size()) return;

  // Take the next edge and try to include it while coloring its nodes Green -
  // Red and then Red - Green (if possible) and also try not including it (if it
  // can find a better result)
  Edge nextEdge = edges.at(index);
  get<3>(nextEdge) = included;
  edges[index] = nextEdge;

  // Try to include this edge and color it's nodes Red - Green
  if (canColorRedGreen(colors, nextEdge)) {
    vector<NodeColor> redgreen(colors);
    redgreen[get<0>(nextEdge)] = red;
    redgreen[get<1>(nextEdge)] = green;
    solveDFS(redgreen, edges, index + 1, chosenWeight + get<2>(nextEdge),
             potentialWeight);
  }

  // Try to include this edge and color it's nodes Green - Red
  if (canColorGreenRed(colors, nextEdge)) {
    vector<NodeColor> greenred(colors);
    greenred[get<0>(nextEdge)] = green;
    greenred[get<1>(nextEdge)] = red;
    solveDFS(greenred, edges, index + 1, chosenWeight + get<2>(nextEdge),
             potentialWeight);
  }

  // Try not to include this edge at all
  if (shouldTryNotAdding(colors, nextEdge)) {
    get<3>(nextEdge) = excluded;
    edges[index] = nextEdge;
    solveDFS(colors, edges, index + 1, chosenWeight,
             potentialWeight - get<2>(nextEdge));
  }
}

/**
 * @brief Wrapper function that sets up the calculation and analytics.
 *
 * @param n - number of nodes
 * @param edges - vector of edges
 * @param colors - vector of colors of nodes
 * @return unsigned int - max weight
 */
unsigned int solve(unsigned int n, vector<Edge> &edges,
                   vector<NodeColor> &colors) {
  // Initiate the variables for the calculation
  sort(edges.begin(), edges.end(), sortByWeight);
  colors.resize(n, c_undefined);
  colors[0] = red;

  // Initiate the variables for the analytics
  clock_t calculationEnd;
  recursionCount = 0;
  maxWeight = 0;
  clock_t calculationStart = clock();

  solveDFS(colors, edges, 0, getChosenWeight(edges), getPotentialWeight(edges));

  calculationEnd = clock();

  // Print the result
  printResult(calculateTime(calculationStart, calculationEnd));
  return maxWeight;
}

/**
 * @brief Main function - gets all inputs, prints outputs.
 *
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return int - status code
 */
int main(int argc, char *argv[]) {
  // Get the inputs from the terminal
  string inputPath = parseArgs(argc, argv);
  if (inputPath == "") return 1;

  // Prepare variables for calculation
  unsigned int n;
  vector<Edge> edges;
  vector<NodeColor> colors;

  // Parse the file and get the structure of the graph
  n = parseFile(inputPath, edges);
  printGraph(n, edges);

  // Run the calculation
  solve(n, edges, colors);

  return 0;
}
