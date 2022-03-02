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

string parseArgs(int argc, char *argv[]) {
  if (argc != 3) return "";
  if (strcmp(argv[1], "-f") != 0) return "";
  return argv[2];
}

unique_ptr<Edge> findEdgeByIds(vector<Edge> edges, uint8_t id1, uint8_t id2) {
  for (auto &e : edges) {
    if (get<0>(e) == id1 && get<1>(e) == id2) return make_unique<Edge>(e);
    if (get<0>(e) == id2 && get<1>(e) == id1) return make_unique<Edge>(e);
  }
  return nullptr;
}

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
        cout << edges.size() << ", i: " << i << ", j: " << j << endl;
      }
      ++j;
    }
    j = 0;
    ++i;
  }
  return n;
}

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

void printEdgeWeights(vector<Edge> &edges) {
  for (auto &e : edges) {
    cout << get<2>(e) << endl;
  }
}

double calculateTime(clock_t &start, clock_t &end) {
  return double(end - start) / CLOCKS_PER_SEC;
}

bool sortByWeight(const Edge &e1, const Edge &e2) {
  return get<2>(e1) > get<2>(e2);
}

unsigned int getChosenWeight(const vector<Edge> &edges) {
  unsigned int sum = 0;
  for (auto &e : edges) {
    if (get<3>(e) == included) sum += get<2>(e);
  }
  return sum;
}

unsigned int getPotentialWeight(const vector<Edge> &edges) {
  unsigned int sum = 0;
  for (auto &e : edges) {
    if (get<3>(e) != excluded) sum += get<2>(e);
  }
  return sum;
}

bool canColorRedGreen(const vector<NodeColor> &colors, const Edge &edge) {
  return colors[get<0>(edge)] != green && colors[get<1>(edge)] != red;
}

bool canColorGreenRed(const vector<NodeColor> &colors, const Edge &edge) {
  return colors[get<0>(edge)] != red && colors[get<1>(edge)] != green;
}

bool shouldTryNotAdding(const vector<NodeColor> &colors, const Edge &edge) {
  NodeColor c1 = colors[get<0>(edge)];
  NodeColor c2 = colors[get<1>(edge)];
  return !((c1 == red && c2 == green) || (c1 == green && c2 == red));
}

void solveDFS(vector<NodeColor> colors, vector<Edge> edges,
              unsigned int index) {
  ++recursionCount;
  if (recursionCount % 100000 == 0) cout << recursionCount << endl;
  unsigned int chosenWeight = getChosenWeight(edges);
  unsigned int potentialWeight = getPotentialWeight(edges);

  // Check if it is connected and bipartite
  if (chosenWeight > maxWeight) maxWeight = chosenWeight;
  if (potentialWeight <= maxWeight || index >= edges.size()) return;

  Edge nextEdge = edges.at(index);
  get<3>(nextEdge) = included;
  edges[index] = nextEdge;
  // Try to color nodes Red - Green
  if (canColorRedGreen(colors, nextEdge)) {
    vector<NodeColor> redgreen(colors);
    redgreen[get<0>(nextEdge)] = red;
    redgreen[get<1>(nextEdge)] = green;
    solveDFS(redgreen, edges, index + 1);
  }

  if (canColorGreenRed(colors, nextEdge)) {
    vector<NodeColor> greenred(colors);
    greenred[get<0>(nextEdge)] = green;
    greenred[get<1>(nextEdge)] = red;
    solveDFS(greenred, edges, index + 1);
  }

  if (shouldTryNotAdding(colors, nextEdge)) {
    get<3>(nextEdge) = excluded;
    edges[index] = nextEdge;
    solveDFS(colors, edges, index + 1);
  }
}

unsigned int solve(unsigned int n, vector<Edge> &edges,
                   vector<NodeColor> &colors) {
  clock_t calculationStart = clock();
  clock_t calculationEnd;
  recursionCount = 0;
  maxWeight = 0;

  // Init
  sort(edges.begin(), edges.end(), sortByWeight);
  colors.resize(n, c_undefined);

  solveDFS(colors, edges, 0);

  calculationEnd = clock();

  cout << endl << "=========================" << endl;
  cout << "Max weight: " << maxWeight << endl;
  cout << "Calculation time: "
       << calculateTime(calculationStart, calculationEnd) << endl;
  cout << "Recursion call count: " << recursionCount << endl;
  return maxWeight;
}

int main(int argc, char *argv[]) {
  string inputPath = parseArgs(argc, argv);
  if (inputPath == "") return 1;

  unsigned int n;
  vector<Edge> edges;
  vector<NodeColor> colors;

  n = parseFile(inputPath, edges);
  printGraph(n, edges);

  solve(n, edges, colors);

  return 0;
}
