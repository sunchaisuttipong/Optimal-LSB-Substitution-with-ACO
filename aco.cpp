#include <iostream>
#include <vector>
#include <tgmath.h>
#include "aco.h"

using namespace std;

ACO::ACO(string cover,string msg,int numBitLSB, bool flag){

  PoB = flag;

  numBit = numBitLSB;
  numNode = pow(2,numBitLSB);

  numTour = 21;
  numAnt = 6;
  ph = 0.5;
  ep = 0.7;
  q = 0.2;

  C = cover ;//Sequence of Cover image's Bit.
  M = msg;//Sequence of Message image's Bit.

  init();//initialize.
  int i = 0;
  while (i < numTour) {
    constructSolution();
    //ASPheromoneUpdate();
    ACSGlobalPheromoneUpdate();
    updateStatistics();
    i++;
  }
  std::cout << "-----------------" << std::endl;
  std::cout << "numAnt = "<< numAnt << std::endl;
  std::cout << "numTour = "<< numTour << std::endl;
  std::cout << "-----------------" << std::endl;

}//ACO.

void ACO::init(){

  //INIT DISTANT.[exp]
  dist.resize(numNode);
  for (size_t i = 0; i < numNode; i++) {
    dist[i].resize(numNode);
  }
  for (size_t i = 0; i < numNode; i++) {
    for (size_t j = 0; j < numNode; j++) {
      dist[i][j] = 1;
    }
  }

  //INIT PHEROMONE.
  pheromone.resize(numNode);
  for (size_t i = 0; i < numNode; i++) {
    pheromone[i].resize(numNode);

    for (size_t j = 0; j < numNode; j++) {
      pheromone[i][j] = ph;
    }
  }//init pheromone.

  //INIT CHOICE_INFO.
  choice_info.resize(numNode);
  for (size_t i = 0; i < numNode; i++) {
    choice_info[i].resize(numNode);

    for (size_t j = 0; j < numNode; j++) {
      choice_info[i][j] = pow(pheromone[i][j],1) * pow((double)1/dist[i][j],1);
    }
  }//init choice_info.

  //init Ant.
  ant.resize(numAnt,single_ant());
  for (size_t i = 0; i < numAnt; i++) {
    ant[i].tour.resize(numNode,0);
    ant[i].visited.resize(numNode);
  }//init Ant.

}//init Function.

void ACO::constructSolution(){

  int step = 0;

  for (size_t i = 0; i < numAnt; i++) {
    for (size_t j = 0; j < numNode; j++) {
      ant[i].visited[j] = false;
    }
  }//for i index.

  for (size_t i = 0; i < numAnt; i++) {
    int r = rand() % numNode;
    ant[i].tour[step] = r;
    ant[i].visited[r] = true;
    ACSLocalPheromoneUpdate(i,step);
  }

  for (size_t i = 1; i < numNode; i++) {

    for (size_t j = 0; j < numAnt; j++) {

      double r = ((double)rand() / RAND_MAX)*1;
      
      if (r <= q) {
        chooseBestNext(j,i);
        ACSLocalPheromoneUpdate(j,i);
      }else{
        ASDecisionRule(j,i);
        ACSLocalPheromoneUpdate(j,i);
      }

    }
  }

  for (size_t i = 0; i < numAnt; i++) {
    ant[i].tour_length = ComputeTourLength(i);
  }

}//constructSolution.

void ACO::chooseBestNext(int noAnt,int step){

  int c;
  double v  = 0;
  for (size_t i = 0; i < numNode; i++) {
    if (!ant[noAnt].visited[i]) {
      if (choice_info[i][step] > v) {
        c = i;
        v = choice_info[i][step];
      }
    }
  }

  ant[noAnt].tour[step] = c;
  ant[noAnt].visited[c] = true;

}//chooseBestNext.

void ACO::ASDecisionRule(int noAnt,int step){

  //int current = ant[noAnt].tour[step-1];

  double sum_probabilities = 0;
  double selection_probability[numNode];

  for (size_t i = 0; i < numNode; i++) {
    if(ant[noAnt].visited[i]){
      selection_probability[i] = 0;
    }else{
      selection_probability[i] = choice_info[i][step];
      sum_probabilities += selection_probability[i];
    }
  }

  double r = ((double)rand() / RAND_MAX)*sum_probabilities;

  int node = 0;
  double p = selection_probability[node];

  while (p < r) {
    node++;
    p += selection_probability[node];
  }
  ant[noAnt].tour[step] = node;
  ant[noAnt].visited[node] = true;

}//ASDecisionRule.

void ACO::ACSLocalPheromoneUpdate(int noAnt, int step){

  int node = ant[noAnt].tour[step];

  pheromone[node][step] = ((1-ep)*pheromone[node][step]) + (ep*ph);
  choice_info[node][step] = pow((pheromone[node][step]),1) * pow(1/(double)dist[node][step],1);
}

void ACO::ACSGlobalPheromoneUpdate(){

  evaporate();

  int noAnt = 0;
  long int tl = ant[0].tour_length;

  for (int i = 0; i < numAnt; i++) {
    if(tl > ant[i].tour_length){
      tl = ant[i].tour_length;
      noAnt = i;
    }
  }
  depositPheromoneBestSoFar(noAnt);
  computeChoiceInformation();

}

void ACO::ASPheromoneUpdate(){

  evaporate();
  for (int i = 0; i < numAnt; i++) {
    depositPheromone(i);
  }
  computeChoiceInformation();

}//ASPheromoneUpdate.

void ACO::evaporate(){
  for (size_t i = 0; i < numNode; i++) {
    for (size_t j = 0; j < numNode; j++) {
      pheromone[i][j] *= (1-ep);
    }
  }
}//evaporate.

void ACO::depositPheromoneBestSoFar(int noAnt){


  for (size_t i = 0; i < numNode; i++) {
    int j = ant[noAnt].tour[i];

    double addPh = ( (1-ep) * pheromone[i][j] ) + (ep * (1/ant[noAnt].tour_length));

    pheromone[i][j] += addPh;
  }

}//depositPheromoneBestSoFar.

void ACO::depositPheromone(int noAnt){

  double addPh = 1/(double)ant[noAnt].tour_length;
  for (size_t i = 0; i < numNode; i++) {
    int j = ant[noAnt].tour[i];
    pheromone[i][j] += addPh;
  }

}//depositPheromone.

void ACO::computeChoiceInformation(){

  for (size_t i = 0; i < numNode; i++) {
    for (size_t j = 0; j < numNode; j++) {
      choice_info[i][j] =  pow(pheromone[i][j],1) * pow(1/(double)dist[i][j],1);
    }
  }

}//computeChoiceInformation.

double ACO::ComputeTourLength(int noAnt){

  double mse = 0;
  if (!collectorTour.empty()) {
    for (size_t i = 0; i < collectorTour.size(); i++) {
      if(collectorTour[i].tour == ant[noAnt].tour){
        mse = collectorTour[i].tour_length;
        break;
      }
    }
    if(mse == 0) goto compute;
  }else{//if never compute that tour.

    compute:

    if(!PoB){

      string Ms = "";

      //Get and Swap.
      for (size_t i = 0; i < M.size(); i+=numBit*numNode) {

        string subC = C.substr(i,numBit*numNode);
        string subM = M.substr(i,numBit*numNode);

        vector<string> swapM(numNode);
        //Swap.
        for (size_t j = 0; j < numBit*numNode; j+= numBit) {

          swapM[j/numBit]= subM.substr(j,numBit);

        }//for j index.

        for (size_t j = 0; j < numNode; j++) {
          Ms += swapM[ant[noAnt].tour[j]];
        }

      }//for i index.

      double sum = 0;

      double l = 0;
      for (size_t i = 0; i < Ms.size(); i+=numBit) {

        string subC = C.substr(i,numBit);
        string subMs = Ms.substr(i,numBit);

        int decC = bitToDec(subC);
        int decMs = bitToDec(subMs);

        sum += pow(decC - decMs,2);
        l++;
      }

      mse = sum / l;

      tour t;
      t.tour = ant[noAnt].tour;
      t.tour_length = mse;
      collectorTour.push_back(t);

    }else{

      string Ms = "";

      //Get and Swap.
      for (size_t i = 0; i < M.size(); i+=numNode) {

        string subC = C.substr(i,numNode);
        string subM = M.substr(i,numNode);

        vector<string> swapM(numNode);
        //Swap.
        for (size_t j = 0; j < numNode; j++) {

          swapM[j]= subM.substr(j,1);

        }//for j index.

        for (size_t j = 0; j < numNode; j++) {
          Ms += swapM[ant[noAnt].tour[j]];
        }

      }//for i index.

      double sum = 0;

      double l = 0;
      for (size_t i = 0; i < Ms.size(); i+=numBit) {

        string subC = C.substr(i,numBit);
        string subMs = Ms.substr(i,numBit);

        int decC = bitToDec(subC);
        int decMs = bitToDec(subMs);

        sum += pow(decC - decMs,2);
        l++;
      }

      mse = sum / l;

      tour t;
      t.tour = ant[noAnt].tour;
      t.tour_length = mse;
      collectorTour.push_back(t);

    }//end else.

  }//end else.

   return mse;

}

void ACO::updateStatistics(){

  iteration++;
  if(bestTour.empty()){
    bestTour.resize(numNode);
    vector<int> initTour(numNode);
    for (size_t i = 0; i < numNode; i++) {
      initTour[i] = i;
    }
    double initTourLength = 0;

    if(!PoB){

      string Ms = "";

      //Get and Swap.
      for (size_t i = 0; i < M.size(); i+=numBit*numNode) {

        string subC = C.substr(i,numBit*numNode);
        string subM = M.substr(i,numBit*numNode);

        vector<string> swapM(numNode);
        //Swap.
        for (size_t j = 0; j < numBit*numNode; j+= numBit) {

          swapM[j/numBit]= subM.substr(j,numBit);

        }//for j index.

        for (size_t j = 0; j < numNode; j++) {
          Ms += swapM[initTour[j]];
        }

      }//for i index.

      double sum = 0;

      double l = 0;
      for (size_t i = 0; i < Ms.size(); i+=numBit) {

        string subC = C.substr(i,numBit);
        string subMs = Ms.substr(i,numBit);

        int decC = bitToDec(subC);
        int decMs = bitToDec(subMs);

        sum += pow(decC - decMs,2);
        l++;
      }

      initTourLength = sum / l;

    }else{

      string Ms = "";

      //Get and Swap.
      for (size_t i = 0; i < M.size(); i+=numNode) {

        string subC = C.substr(i,numNode);
        string subM = M.substr(i,numNode);

        vector<string> swapM(numNode);
        //Swap.
        for (size_t j = 0; j < numNode; j++) {

          swapM[j]= subM.substr(j,1);

        }//for j index.

        for (size_t j = 0; j < numNode; j++) {
          Ms += swapM[initTour[j]];
        }

      }//for i index.

      double sum = 0;

      double l = 0;
      for (size_t i = 0; i < Ms.size(); i+=numBit) {

        string subC = C.substr(i,numBit);
        string subMs = Ms.substr(i,numBit);

        int decC = bitToDec(subC);
        int decMs = bitToDec(subMs);

        sum += pow(decC - decMs,2);
        l++;
      }

      initTourLength = sum / l;

    }//end else.

    bestTour = initTour;
    bestTourLength = initTourLength;


    std::cout << "init Best tour" ;
    for (size_t i = 0; i < numNode; i++) {
      std::cout << bestTour[i];
    }
    std::cout  << std::endl;
    std::cout << "inti Tour_Length : "<< bestTourLength << std::endl;

  }//end if.

  for (size_t i = 0; i < numAnt; i++) {

    //Find Best Tour.
    if (bestTourLength > ant[i].tour_length) {

      double pvBestTourLength = bestTourLength;
      vector<int> pvBestTour = bestTour;

      bestTourLength = ant[i].tour_length;
      bestTour = ant[i].tour;

    }
  }//for i index.

}

int ACO::bitToDec(string bit){
  int dec = 0;

  for (int i = bit.size()-1; i >= 0; i--) {
    if (bit[(bit.size()-1)-i]=='1') {
      dec += pow(2,i);
    }
  }
  return dec;
}
