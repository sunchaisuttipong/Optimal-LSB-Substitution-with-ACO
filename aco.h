#include <vector>
#include <iostream>
using namespace std;

class ACO{

  vector<vector<double> > dist;
  vector<vector<double> > pheromone;
  vector<vector<double> > choice_info;

  struct single_ant{

    double tour_length;
    vector<int> tour;
    vector<bool> visited;

  };
  struct tour{

    vector<int> tour;
    double tour_length;

  };
  int numTour;
  int numBit;
  int numAnt;
  int numNode;//Number of Node.

  double ph;//the initial value of Pheromones
  double ep;//the parameter of the evaporation of Pheromones
  double q;//the random number parameter

  string C;
  string M;

  vector<single_ant> ant;//arrey of ant's struct.
  vector<tour> collectorTour;

  bool PoB;//false for Pixel position and true for Bit position.

public:

  int iteration;
  double bestTourLength;
  vector<int> bestTour;

  ACO(string,string,int,bool);

private:

  void init();
  void constructSolution();
  void chooseBestNext(int,int);
  void ASDecisionRule(int, int);
  void ACSLocalPheromoneUpdate(int,int);
  void ACSGlobalPheromoneUpdate();
  void ASPheromoneUpdate();
  void evaporate();
  void depositPheromoneBestSoFar(int);
  void depositPheromone(int);
  void computeChoiceInformation();
  double ComputeTourLength(int);
  void updateStatistics();
  int bitToDec(string);

};
