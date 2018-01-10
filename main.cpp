//
//  main.cpp
//  CompEvolve
//
//  Created by Carl Geiger on 7/17/17.
//  
//


#include<iostream>
#include<algorithm>
#include<utility>
#include<string>
#include<vector>
#include<map>
#include<cmath>
#include<random>

using namespace std;


//given a genome length, makes a random Genome sequence
vector<int> initializeGenome(int length){
    vector<int> sequence={};
    for(int i=0;i<length;i++){
        random_device seed;
        default_random_engine dre(seed());
        uniform_int_distribution<int> dist(0,9);
        int amino=dist(dre);
        sequence.push_back(amino);
    }
    return sequence;
}

vector<vector<int>> optimizeGenome(vector<pair<vector<int>,int>> Genomes, int elitism){
    vector<vector<int>> bestGenomes;
    
    //finds the maximum score of the genome list and creates a vector of the best scoring genomes given a length (elitism).
    for(int k=0;k<elitism;k++){
        int maxScore=0;
        int maxIndex=0;
        vector<int>bestGenome={0};
        for(int l=0;l<Genomes.size();l++){
            if(Genomes[l].second>maxScore){
                maxScore=Genomes[l].second;
                bestGenomes.push_back(Genomes[l].first);
                maxIndex=l;
            }
            
        }
        Genomes.erase(Genomes.begin()+maxIndex);
    }
    
    return bestGenomes;
}

//function used to mutate genomes
vector<int> mutateGenome(vector<int> Genome, double mutationRate){
vector<int>nwGenomes;
    //point mutation
    for(auto element : Genome){
        nwGenomes.push_back(element);
    }
        
    for(int g=0;g<Genome.size();g++){
        random_device seed;
        default_random_engine dre(seed());
        uniform_real_distribution<double> dist(0,100);
        double roll=dist(dre);
        uniform_int_distribution<int> rng(0,9);
        if(roll<=mutationRate){
            nwGenomes[g]=rng(dre);
        }
    }
    return nwGenomes;
}

//populates the world with (potentially) mutated genomes
vector<vector<int>> populateWorld(vector<vector<int>> Genomes, int popSize, double mutationRate){
    vector<vector<int>> Orgs={};
    while(Orgs.size()<popSize){
        vector<vector<int>> mutGenomes={};
        for(auto element : Genomes){
            mutGenomes.push_back(mutateGenome(element,mutationRate));
        }
        for(auto element : mutGenomes){
            //makes sure the population size hasn't maxed out
            if(Orgs.size()<popSize){
                Orgs.push_back(element);
            }
            
        }
    }
    
    
    return Orgs;
}

//The function that determines how an organism thinks.
int brainOutput(vector<int> genome,int input){
    int output=0;
    int total=0;
    for(auto element : genome){
        total+=element;
    }
    
    if(total<genome.size()*2){
        random_device seed;
        default_random_engine dre(seed());
        uniform_int_distribution<int> dist(0,1);
        int roll=dist(dre);
        output=roll;
    }
    else if(genome[0]>genome[genome.size()-1]){
        output=(input==0)?(1):0;
        
    }
    else{
        output=(input==0)?(0):1;
    }
    return output;
}
//GenomeWorld adds up the ints in the genome and returns the total as the score.
int GenomeWorld(vector<int> Genome){
    int tot=0;
    for (auto element : Genome){
        tot+=element;
    }
    if(tot<9*Genome.size()){
        brainOutput(Genome, 1);
    }
    else{
        brainOutput(Genome,0);
    }
    return tot;
}

int RepeatWorld(vector<int> Genome){
    int score=0;
    int updates=20;
    for(int i=0;i<updates;i++){
        random_device seed;
        default_random_engine dre(seed());
        uniform_int_distribution<int> dist(0,1);
        int input=dist(dre);
        int output=brainOutput(Genome, input);
        if (input==output){
            score++;
        }
    }
    return score;
}

//TreasureWorld has the organism find treasure.
int TreasureWorld(vector<int> Genome){
    
    //creates the organism genome
    
    //X marks the spot!  X is the coordinates of the treasure.  The island is a 100 length square and wraps around.
    vector<int> X={70,80};
    //the Organism always starts at the bottom left corner.
    vector<int>orgLoc={1,1};
    
    //the organism has 200 steps to go left or right, and 200 steps to go up or down.  It's allowed to move left or right first, and then up and down.
    for(int n=0;n<2;n++){
        for(int i=0;i<200;i++){
            
            
            //If the organism has passed the treasure coordinate, a 1 is inputted to the brain, otherwise a 0 is inputted to the brain.
            int output=(orgLoc[n]<X[n])?(brainOutput(Genome,0)):(brainOutput(Genome,1));
            
            //If the brain outputs a 1, it goes up/right, if it outputs a 0 it goes down/left)
            (output==1)?(orgLoc[n]++):(orgLoc[n]--);
            if(orgLoc[n]>100){
                orgLoc[n]-=100;
            }
            if(orgLoc[n]<1){
                orgLoc[n]+=100;
            }
            
        }
    }
    int Xdist=abs(orgLoc[0]-X[0]);
    int Ydist=abs(orgLoc[1]-X[1]);
    return 200-Xdist-Ydist;
}

int main() {
    int genomeLength; int popSize; vector<vector<int>> orgs; int updates; int elitism; double mutationRate; vector<vector<int>> allScores; vector<double> aveScores;
    
    //CREATE THE FIRST GENERATION
//    cout<<"Enter a length for the genomes: ";
//    cin>>genomeLength;
//    cout<<endl<<"Enter a population size: ";
//    cin>>popSize;
//    cout<<endl;
    genomeLength=5;
    popSize=100;
    for(int p=0;p<popSize;p++){
        orgs.push_back(initializeGenome(genomeLength));
    }
    
    //preliminary inputs
    
    updates=1000;
    elitism=10;
    mutationRate=1;
    
    
//    cout<<"Enter the number of generation updates: ";
//    cin>>updates;
//    cout<<endl;
//    cout<<"Enter elitism (how many genomes that create offspring per generation): ";
//    cin>>elitism;
//    cout<<endl;
//    cout<<"Enter a mutation rate: ";
//    cin>>mutationRate;
//    cout<<endl;
    
    //THE GENERATION CYCLE
    for(int u=0;u<updates;u++){
        //PART 1: WORLD EVAULATION
        vector<pair<vector<int>,int>>evald_Orgs;
        for(int y=0;y<orgs.size();y++){
            evald_Orgs.push_back(make_pair(orgs[y],TreasureWorld(orgs[y])));
        }
        //PART 1.5: DATA COLLECTION
        
        int max_score = 0;
        vector<int> scores={};
        int tot=0;
        for(auto element : evald_Orgs){
            scores.push_back(element.second);
            tot+=element.second;
            if(element.second>max_score){
                max_score=element.second;
            }
        }
        aveScores.push_back(tot/evald_Orgs.size());
        allScores.push_back(scores);
        cout<<"Update: "<<u<<"    Score: "<<max_score<<endl;
        
        //PART 2: OPTIMIZATION
        vector<vector<int>>optGenomes=optimizeGenome(evald_Orgs, elitism);
        
        //PART 3: CREATE NEW OFFSPRING
        orgs=populateWorld(optGenomes, popSize, mutationRate);
        
    }
}
