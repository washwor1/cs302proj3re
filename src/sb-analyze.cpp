#include "disjoint_set.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <iostream>
#include <set>
#include <vector>
#include <iomanip>
using namespace std;
using plank::Disjoint_Set;

#define talloc(type, num) (type *) malloc(sizeof(type)*(num))

class Superball {
  public:
    Superball(int argc, char **argv);
    int r;
    int c;
    int mss;
    int empty;
    vector <int> board;
    vector <int> goals;
    vector <int> colors;
    Disjoint_Set *ds;
    void analyzeSuperball();
};

void usage(const char *s) 
{
  fprintf(stderr, "usage: sb-read rows cols min-score-size colors\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

Superball::Superball(int argc, char **argv)
{
  int i, j;
  string s;

  if (argc != 5) usage(NULL);

  if (sscanf(argv[1], "%d", &r) == 0 || r <= 0) usage("Bad rows");
  if (sscanf(argv[2], "%d", &c) == 0 || c <= 0) usage("Bad cols");
  if (sscanf(argv[3], "%d", &mss) == 0 || mss <= 0) usage("Bad min-score-size");

  colors.resize(256, 0);

  for (i = 0; i < strlen(argv[4]); i++) {
    if (!isalpha(argv[4][i])) usage("Colors must be distinct letters");
    if (!islower(argv[4][i])) usage("Colors must be lowercase letters");
    if (colors[argv[4][i]] != 0) usage("Duplicate color");
    colors[argv[4][i]] = 2+i;
    colors[toupper(argv[4][i])] = 2+i;
  }

  board.resize(r*c);
  goals.resize(r*c, 0);
  ds = new Disjoint_Set;
  ds->Initialize(r*c);



  empty = 0;

  for (i = 0; i < r; i++) {
    if (!(cin >> s)) {
      fprintf(stderr, "Bad board: not enough rows on standard input\n");
      exit(1);
    }
    if (s.size() != c) {
      fprintf(stderr, "Bad board on row %d - wrong number of characters.\n", i);
      exit(1);
    }
    for (j = 0; j < c; j++) {
      if (s[j] != '*' && s[j] != '.' && colors[s[j]] == 0) {
        fprintf(stderr, "Bad board row %d - bad character %c.\n", i, s[j]);
        exit(1);
      }
      board[i*c+j] = s[j];
      if (board[i*c+j] == '.') empty++;
      if (board[i*c+j] == '*') empty++;
      if (isupper(board[i*c+j]) || board[i*c+j] == '*') {
        goals[i*c+j] = 1;
        board[i*c+j] = tolower(board[i*c+j]);
      }
    }
  }
}

int main(int argc, char **argv)
{
  Superball s(argc, argv);
  int ngoal, tgoal;
    tgoal = 0;
    ngoal = 0;
    
    //adds the colors in
    for (int i = 0; i < s.r*s.c; i++) {
      if (s.goals.at(i) && s.board.at(i) != '*') {
        tgoal += s.colors.at(s.board.at(i));
        ngoal++;
      }
  }
  //anaylze the superball through the restraints
  s.analyzeSuperball();

}


void Superball::analyzeSuperball() {
  vector<int> rank;
  vector<bool> print;
  rank.resize(board.size(), 1);
  print.resize(board.size(), false);

  //loops through the entire board and at every cell, checks if the bottom and right cells are the same color and unions them and adds their rank if that is the case.
  for (unsigned int i = 0; i<board.size(); i++) {
    int s1,s2,s3;
    //checks if the cell is empty
    if(board.at(i) != '.' && board.at(i) != '*'){
      //adds the right if it exists
      if(i%c != c-1 && board.at(i) == board.at(i+1)){
          s1 = ds->Find(i);
          s2 = ds->Find(i+1);
          if(s1 != s2){
            s3 = ds->Union(s1, s2);
            rank[s3] = rank[s2] + rank[s1];
          }
        }
      //adds the bottom if it exists
      if(i/c!=r-1 && board.at(i) == board.at(i+c)){
        s1 = ds->Find(i);
        s2 = ds->Find(i+c);
        if(s1 != s2){
          s3 = ds->Union(s1, s2);  
          rank[s3] = rank[s2] + rank[s1];
        }
      }
    }
  }

  //output section 
  cout << "Scoring sets:" << endl;
  for(int i = 0; i < board.size();i++){
    if(goals.at(i) == 1 && board.at(i) != '.' && board.at(i) != '*'){
      int root = ds->Find(i);
      if(rank[root] >= mss && print[root] == false){
        print[root] = true;
        cout << " Size: " << setw(2) << setfill(' ') << rank[root] << " Char: " << (char)board.at(i) << " Scoring Cell: " << i/c << "," << i%c << endl;
      } 
    }
  }
  
}

