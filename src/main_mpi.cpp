#include <omp.h>
#include <stdio.h>

#include <algorithm>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "mpi.h"

#define NODE_SIZE 20
#define EDGE_SIZE 60

using namespace std;

unsigned int maxWeight;
enum EdgeState : uint8_t { s_undefined, included, excluded };
enum NodeColor : uint8_t { c_undefined, red, green };
using Edge = tuple<uint8_t, uint8_t, unsigned int, EdgeState>;

int tag_new_work = 1;
int tag_work_done = 2;
int tag_finished = 0;

struct Args {
  bool ok;
  string input;
  int threads;
};

struct Message {
  uint8_t colors[NODE_SIZE];
  uint8_t edgesId1[EDGE_SIZE];
  uint8_t edgesId2[EDGE_SIZE];
  unsigned int edgesWeight[EDGE_SIZE];
  uint8_t edgesState[EDGE_SIZE];
  unsigned int node_length;
  unsigned int edge_length;
  unsigned int index;
  unsigned int chosenWeight;
  unsigned int potentialWeight;
  unsigned int maxWeight;
};

struct State {
  vector<NodeColor> colors;
  vector<Edge> edges;
  unsigned int index;
  unsigned int chosenWeight;
  unsigned int potentialWeight;
  unsigned int maxWeight;

  State(vector<NodeColor> colors, vector<Edge> edges, unsigned int index,
        unsigned int chosenWeight, unsigned int potentialWeight,
        unsigned int maxWeight = 0) {
    this->colors = colors;
    this->edges = edges;
    this->index = index;
    this->chosenWeight = chosenWeight;
    this->potentialWeight = potentialWeight;
    this->maxWeight = maxWeight;
  }

  State() {}

  void setMaxWeight(unsigned int maxWeight) { this->maxWeight = maxWeight; }
};

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
  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < n; ++j) {
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
 * @brief Prints weight, time
 *
 * @param time
 */
void printResult(double time) {
  cout << "Max weight: " << maxWeight << endl;
  cout << "Calculation time: " << time << endl;
}
void printResult(double time, string title) {
  cout << title << " took time: " << time << endl;
}

/**
 * @brief Finds the node that has the most edges that are incident with it
 *
 * @param n - number of nodes
 * @param edges - vector of edges
 * @return uint8_t - id of the node
 */
uint8_t findNodeWithMostEdges(unsigned int n, const vector<Edge> &edges) {
  vector<unsigned int> counts(n, 0);
  unsigned int maxCount = 0;
  uint8_t maxNodeId = 0;
  for (const auto &e : edges) {
    unsigned int count = counts[get<0>(e)];
    ++count;
    if (count > maxCount) {
      maxCount = count;
      maxNodeId = get<0>(e);
    }
  }
  return maxNodeId;
}

/**
 * @brief Parses args from terminal.
 *
 * @param argc - number of arguments
 * @param argv - array of arguments
 * @return Args - returns struct Args containing all the arguments
 */
Args parseArgs(int argc, char *argv[]) {
  Args args;
  args.ok = true;
#pragma omp parallel
  { args.threads = omp_get_num_threads(); }
  if (argc < 3 || argc % 2 != 1) args.ok = false;
  for (int i = 1; i < argc && args.ok; ++i) {
    if (strcmp(argv[i], "-f") == 0) args.input = argv[i + 1];
    if (strcmp(argv[i], "-t") == 0) args.threads = stoi(argv[i + 1]);
  }
  return args;
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
double calculateTime(double start, double end) { return end - start; }

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
  // Check if it is connected and overwrite best result if current is better
  // (the graph is bipartite, no need to check that)
  if (chosenWeight > maxWeight && isConnected(colors.size(), edges)) {
#pragma omp critical
    {
      if (chosenWeight > maxWeight) maxWeight = chosenWeight;
    }
  }

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
 * @brief BFS algorithm to generate states
 *
 * @param init - initial state from which BFS starts
 * @param statesToGenerate - number of states that should be generated
 * @return queue<State> - final queue of all the states
 */
queue<State> generateStatesQueue(State &init,
                                 unsigned int statesToGenerate = 50) {
  queue<State> q;
  q.push(init);
  unsigned int numberOfStates = q.size();
  while (numberOfStates < statesToGenerate && !q.empty()) {
    State front = q.front();
    q.pop();

    if (front.chosenWeight > maxWeight &&
        isConnected(front.colors.size(), front.edges)) {
      maxWeight = front.chosenWeight;
    }

    // Check if we are at the end of the calculation or if there is still a
    // potential to beat the best score (weight)
    if (front.potentialWeight <= maxWeight || front.index >= front.edges.size())
      continue;

    // Take the next edge and try to include it while coloring its nodes Green -
    // Red and then Red - Green (if possible) and also try not including it (if
    // it can find a better result)
    Edge nextEdge = front.edges.at(front.index);
    get<3>(nextEdge) = included;
    front.edges[front.index] = nextEdge;

    // Try to include this edge and color it's nodes Red - Green
    if (canColorRedGreen(front.colors, nextEdge)) {
      vector<NodeColor> redgreen(front.colors);
      redgreen[get<0>(nextEdge)] = red;
      redgreen[get<1>(nextEdge)] = green;
      q.emplace(redgreen, front.edges, front.index + 1,
                front.chosenWeight + get<2>(nextEdge), front.potentialWeight);
    }

    // Try to include this edge and color it's nodes Green - Red
    if (canColorGreenRed(front.colors, nextEdge)) {
      vector<NodeColor> greenred(front.colors);
      greenred[get<0>(nextEdge)] = green;
      greenred[get<1>(nextEdge)] = red;
      q.emplace(greenred, front.edges, front.index + 1,
                front.chosenWeight + get<2>(nextEdge), front.potentialWeight);
    }

    // Try not to include this edge at all
    if (shouldTryNotAdding(front.colors, nextEdge)) {
      get<3>(nextEdge) = excluded;
      front.edges[front.index] = nextEdge;
      q.emplace(front.colors, front.edges, front.index + 1, front.chosenWeight,
                front.potentialWeight - get<2>(nextEdge));
    }

    numberOfStates = q.size();
  }

  return q;
}

/**
 * @brief Generate states into a vector
 *
 * @param states - final vector
 * @param init - initial state for BFS
 * @param statesToGenerate - number of states that should be generated
 */
void generateStates(vector<State> &states, State &init,
                    unsigned int statesToGenerate = 50) {
  queue<State> q = generateStatesQueue(init, statesToGenerate);
  printf("Slave queue size - %lu\n", q.size());
  while (!q.empty()) {
    states.emplace_back(q.front());
    q.pop();
  }
  print("Slave vector size - %lu\n", states.size());
}

/**
 * @brief Check if message array is big enough
 *
 * @param needed - number of node colors to be stored
 */
void checkColorsSize(unsigned int needed) {
  if (needed > NODE_SIZE) {
    cout << "Color array too small: " << NODE_SIZE << " need:" << needed
         << endl;
    throw std::runtime_error("Color array too small");
  }
}

/**
 * @brief Check if message array is big enough
 *
 * @param needed - number of edges to be stored
 */
void checkEdgesSize(unsigned int needed) {
  if (needed > EDGE_SIZE) {
    cout << "Edge array too small: " << EDGE_SIZE << " need:" << needed << endl;
    throw std::runtime_error("Edge array too small");
  }
}

/**
 * @brief Convert state to a message
 *
 * @param state - state to be converted
 * @return Message
 */
Message stateToMessage(State &state) {
  Message message;
  message.chosenWeight = state.chosenWeight;
  message.index = state.index;
  message.maxWeight = state.maxWeight;
  message.potentialWeight = state.potentialWeight;
  message.node_length = state.colors.size();
  message.edge_length = state.edges.size();
  // checkColorsSize(state.colors.size());
  // checkEdgesSize(state.edges.size());
  int i = 0;
  for (auto c : state.colors) {
    message.colors[i] = c;
    ++i;
  }
  i = 0;
  for (auto e : state.edges) {
    message.edgesId1[i] = get<0>(e);
    message.edgesId2[i] = get<1>(e);
    message.edgesWeight[i] = get<2>(e);
    message.edgesState[i] = get<3>(e);
    ++i;
  }
  return message;
}

/**
 * @brief Convert message struct to state struct
 *
 * @param message - message to convert
 * @return State
 */
State messageToState(Message &message) {
  State state;
  state.chosenWeight = message.chosenWeight;
  state.index = message.index;
  state.maxWeight = message.maxWeight;
  state.potentialWeight = message.potentialWeight;
  // Go through all node colors and copy them
  for (unsigned int i = 0; i < message.node_length; ++i) {
    state.colors.push_back(static_cast<NodeColor>(message.colors[i]));
  }
  // Go through all edge indices and make tuple and store it in vector
  for (unsigned int i = 0; i < message.edge_length; ++i) {
    Edge e = make_tuple(message.edgesId1[i], message.edgesId2[i],
                        message.edgesWeight[i],
                        static_cast<EdgeState>(message.edgesState[i]));
    state.edges.push_back(e);
  }
  return state;
}

/**
 * @brief Wrapper function that sets up the calculation and analytics - master
 * process
 *
 * @param n - number of nodes
 * @param edges - vector of edges
 * @param colors - vector of colors of nodes
 * @param rank - rank of master (should be 0)
 * @param processes - number of processes
 * @return unsigned int - max weight
 */
unsigned int solveMaster(unsigned int n, vector<Edge> &edges,
                         vector<NodeColor> &colors, int rank, int processes) {
  // Initiate the variables for the calculation
  sort(edges.begin(), edges.end(), sortByWeight);
  colors.resize(n, c_undefined);

  // Initiate the variables for the analytics
  maxWeight = 0;
  double calculationStart, calculationEnd;
  calculationStart = omp_get_wtime();

  uint8_t maxNodeId = findNodeWithMostEdges(n, edges);
  colors[maxNodeId] = red;
  printf("Master generating\n");
  // Generate states
  queue<State> states;
  State init = State(colors, edges, 0, getChosenWeight(edges),
                     getPotentialWeight(edges));
  states = generateStatesQueue(init, processes * 10);
  printf("Master generated %lu states\n", states.size());
  // Master is already working
  uint8_t workingProcesses = 1;
  unsigned int solution;
  MPI_Status status;
  State current;
  Message message;

  // Send work to all slaves (initial send)
  while (!states.empty() && workingProcesses < processes) {
    current = states.front();
    message = stateToMessage(current);
    MPI_Send(&message, sizeof(message), MPI_PACKED, workingProcesses,
             tag_new_work, MPI_COMM_WORLD);
    states.pop();
    ++workingProcesses;
  }

  // Receive results and send work again until the queue is not empty
  while (!states.empty()) {
    printf("Master - sending work - %lu left\n", states.size());
    // Receive result and update global max
    MPI_Recv(&solution, sizeof(solution), MPI_INT, MPI_ANY_SOURCE,
             tag_work_done, MPI_COMM_WORLD, &status);
    if (solution > maxWeight) {
      maxWeight = solution;
    }

    // Get new state and send it to the slave
    current = states.front();
    current.setMaxWeight(maxWeight);
    message = stateToMessage(current);
    MPI_Send(&message, sizeof(message), MPI_PACKED, status.MPI_SOURCE,
             tag_new_work, MPI_COMM_WORLD);
    states.pop();
  }

  printf("Master - work done\n");
  // All work is done -> get results from slaves + tell them the work is
  // done
  while (workingProcesses > 1) {
    MPI_Recv(&solution, sizeof(solution), MPI_INT, MPI_ANY_SOURCE,
             tag_work_done, MPI_COMM_WORLD, &status);
    if (solution > maxWeight) {
      maxWeight = solution;
    }
    MPI_Send(&workingProcesses, 1, MPI_INT, status.MPI_SOURCE, tag_finished,
             MPI_COMM_WORLD);
    --workingProcesses;
  }

  // Print the result
  calculationEnd = omp_get_wtime();
  printResult(calculateTime(calculationStart, calculationEnd));
  return maxWeight;
}

/**
 * @brief Wrapper function that sets up the calculation for slaves
 *
 * @param rank - rank of the slave (id)
 */
void solveSlave(int rank) {
  MPI_Status status;
  Message message;
  unsigned int numberOfStatesToGenerate;
#pragma omp parallel
  { numberOfStatesToGenerate = omp_get_num_threads() * 10; }

  // Main loop for calculation
  while (true) {
    MPI_Recv(&message, sizeof(message), MPI_PACKED, 0, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);

    // If work is finished then break the loop and end
    if (status.MPI_TAG == tag_finished) {
      printf("Slave #%d - done\n", rank);
      break;
    } else if (status.MPI_TAG == tag_new_work) {
      printf("Slave #%d - received work\n", rank);
      // Get state from message
      State state = messageToState(message);
      // Update local max to global max
      if (state.maxWeight > maxWeight) {
        maxWeight = state.maxWeight;
      }
      // Generate states
      vector<State> states;
      generateStates(states, state, numberOfStatesToGenerate);

      printf("Slave %d - generated %lu(%u) states\n", rank, states.size(),
             numberOfStatesToGenerate);
      // Do data parallelism on generated states
#pragma omp parallel for default(shared)
      for (long unsigned int i = 0; i < states.size(); ++i) {
        State s = states.at(i);
        solveDFS(s.colors, s.edges, s.index, s.chosenWeight, s.potentialWeight);
      }

      // Send back result
      printf("Slave #%d - work done, max: %u\n", rank, maxWeight);
      MPI_Send(&maxWeight, 1, MPI_INT, 0, tag_work_done, MPI_COMM_WORLD);
    } else {
      cout << "ERROR: Unknown tag:" << status.MPI_TAG << endl;
    }
  }
  return;
}

void printStart(const string &input, int processes) {
  unsigned int numberOfThreads;
#pragma omp parallel
  { numberOfThreads = omp_get_num_threads(); }
  cout << "===============================================" << endl;
  cout << "------------ MPI CALCULATION START ------------" << endl;
  cout << "----------------- THREADS: " << numberOfThreads
       << " -----------------" << endl;
  cout << "---------------- PROCESSES: " << processes << " ----------------"
       << endl;
  cout << "-------- INPUT: " << input << " --------" << endl;
  cout << "===============================================" << endl;
}

void printSlave(int rank) {
  unsigned int numberOfThreads;
#pragma omp parallel
  { numberOfThreads = omp_get_num_threads(); }
  printf("  -- SLAVE #%d - THREADS: %d --  \n", rank, numberOfThreads);
}

void printEnd() {
  cout << "===============================================" << endl;
  cout << "-------------- MPI CALCULATION END ------------" << endl;
  cout << "===============================================\n" << endl;
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
  Args args = parseArgs(argc, argv);
  if (!args.ok || args.threads <= 0 || args.input == "") return 1;
  omp_set_num_threads(args.threads);

  // Init MPI and split code into Master and Slaves
  MPI_Init(&argc, &argv);
  int rank, processes;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processes);

  // Split processes into master and slaves
  if (rank == 0) {
    // Master - split work between processes, print all results, measure time
    printStart(args.input, processes);

    // Prepare variables for calculation
    unsigned int n;
    vector<Edge> edges;
    vector<NodeColor> colors;

    // Parse the file and get the structure of the graph
    n = parseFile(args.input, edges);
    // Run the calculation
    solveMaster(n, edges, colors, rank, processes);

    printEnd();
  } else {
    // Slaves - wait for work, solve, send back result
    printSlave(rank);
    solveSlave(rank);
  }
  MPI_Finalize();

  return 0;
}
