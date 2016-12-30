#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>
#include <cassert>
#include <cstring>
#include <cmath>
using namespace std;



class eval_vect
{
    public:
    eval_vect(int num_play_) __attribute__((always_inline)) :num_play(num_play_){memset(eval,0,4*sizeof(int));}
    
    inline float& operator()(int play) __attribute__((always_inline))
      {
        assert(play < num_play); 
        return eval[play];
       }

    inline float operator()(int play) const __attribute__((always_inline))
     {
      assert(play < num_play); 
      return eval[play];
     }
     
eval_vect& operator+=(const eval_vect& rhs)  __attribute__((always_inline))
  {      
    for(int i=0;i<num_play;++i)
    {
      eval[i]+=rhs.eval[i];
    }
  
    return *this; 
  }
 
 
  friend eval_vect operator+(eval_vect lhs, 
                    const eval_vect& rhs) __attribute__((always_inline))
  {
     lhs += rhs;
     return lhs;
  }
  
  eval_vect& operator-=(const eval_vect& rhs)  __attribute__((always_inline))
  {      
    for(int i=0;i<num_play;++i)
    {
      eval[i]-=rhs.eval[i];
    }
  
    return *this; 
  }
 
 
  friend eval_vect operator-(eval_vect lhs, 
                    const eval_vect& rhs) __attribute__((always_inline))
  {
     lhs -= rhs;
     return lhs;
  }
  
   friend eval_vect operator*(float lhs,const eval_vect& rhs) __attribute__((always_inline))
  {
     eval_vect tmp(rhs.num_play);
     for(int i=0;i<rhs.num_play;++i)
     {
        tmp.eval[i]=lhs*rhs.eval[i]; 
     }
     return tmp;
  }
  
   void assign(float value)
   {
       for(int i=0;i<num_play;++i)
         eval[i] = value;
   }
   
   friend ostream& operator<<(ostream& os, const eval_vect& ev) ;
    private:
    int num_play;
    float eval[4];
};

struct coord
{
    coord(int _x,int _y)__attribute__((always_inline)) :x(_x),y(_y) 
  {}
      coord()__attribute__((always_inline)) :x(0),y(0) 
  {}
   // coord():x(0),y(0){}
   
    void print_result() const {cout << x << " " << y << endl;}
    
    int distance(const coord& c) const __attribute__((always_inline))
    {
        return abs(c.x - x) + abs(c.y - y); //manathan distance
    }
    int x,y;
};

ostream& operator<<(ostream& os, const coord& c) 
    {
        os << '('<<c.x<<','<<c.y<<')';
        return os;
    }
    

    
ostream& operator<<(ostream& os, const eval_vect& ev) 
    {
       os << '['; 
        for(int i=0;i<ev.num_play;++i)
        {
            os << ev.eval[i] <<',';
        }
        os << ']';
        return os;
    }
    
    
class board_v
{
    public:
      inline int& operator()(const coord& c) __attribute__((always_inline))
      {
        assert(c.x<35 && c.y<20); 
        return board_val[c.x][c.y];
       }

    inline int operator()(const coord& c) const __attribute__((always_inline))
     {
         assert(c.x<35 && c.y<20); 
      return board_val[c.x][c.y];
     }
     
   inline int& operator()(int x,int y) __attribute__((always_inline))
      {
        assert(x<35 && y<20); 
        return board_val[x][y];
       }

    inline int operator()(int x,int y) const __attribute__((always_inline))
     {
         assert(x<35 && y<20); 
      return board_val[x][y];
     }

  void print_board() const
  {
    cerr << "coord " <<play_coord[0] << endl;
    for(int i = 0;i<20;++i)
      {
	
	for(int j=0;j<35;++j)
	  {
	      cerr << board_val[j][i];
	  }
	cerr << endl;
      }
  }
  //vector<coord> play_coord;
  coord play_coord[4];
    private:
      int board_val[35][20];
     
};
class bttc_b
{
    public:
        bttc_b(int round_,
              int opponentCount_,
             int left_,
             coord p_coord_[4],
             vector<string> &lines):round(round_),left(left_),num_players(opponentCount_+1)
             {
	       //board.play_coord = p_coord_;
		 memcpy(board.play_coord,p_coord_,4*sizeof(coord));
                 for(int i = 0;i<20;++i)
                 {
                    stringstream ss;
                    ss << lines[i];

                    for(int j=0;j<35;++j)
                        {
                            char val;
                            ss >> val;
                            
                            switch(val)
                            {
                                case '.':
                                  board(j,i) = 9;
                                  break;
                                case '0':
                                  board(j,i)= 0;   
                                  break;
                                case '1':
                                  board(j,i) = 1;
                                  break;
                                case '2':
                                  board(j,i)= 2;
                                  break;                                
                                case '3':
                                  board(j,i) = 3;
                                  break;
                            }
                        }
                 }
             }
         
        coord find_next_move() const;             
      
  //  private:
  inline void generate_move_state(list<board_v> &lb,const board_v &curr_board,int player) const __attribute__((always_inline));
  inline void valid_moves(list<coord> &lvm,const coord &c) const __attribute__((always_inline));
  inline eval_vect max_value(int depth,const board_v& curr_board, coord &best_move,int player,int num_play) const;
  inline eval_vect eval_function(const board_v& b,int num_play) const __attribute__((always_inline));
  void detect_area(board_v& b, vector<pair<int, vector<int>>> &vpvlc, int num_play) const;      
  void area_analysis(const coord& c,board_v& b,int &area_size,vector<int > &vlc, int num_play) const;

  int round,left,num_players;
  board_v board;
     
};


coord bttc_b::find_next_move() const
{
   coord best_move(0,0);
   eval_vect val = max_value(5,board,best_move,0, num_players);
   cerr << val << " curr pos " << board.play_coord[0] << " obj : "<<  best_move<< endl;
   return best_move;
}


eval_vect bttc_b::eval_function(const board_v& b,int num_play) const
{
 
    

   eval_vect f1(num_play); // first feature : number of case we can get    
   eval_vect f2(num_play);  //f2 dist from empty case
   
  // f2.assign(10000);
    for(int j=0;j<35;++j)
      for(int i = 0;i<20;++i)             
         {
            coord cur_co(j,i);
            for(int p=0;p<num_play;++p)
              {
                if(b(cur_co) == p || b(cur_co) == p+4)
                  {
                   f1(p) += 1;
                  }
                if( b(cur_co) == 9) 
                {
                    int dist = cur_co.distance(b.play_coord[p]);
                    //cerr <<  " dist " << dist << endl;
                    if(53-dist > f2(p))
                       f2(p) = 53-dist;
                }
               }
         
         }

    vector<pair<int, vector<int>>> vpvlc;
    board_v bt = b;
    detect_area(bt, vpvlc,num_play);
       
  //cerr << b.play_coord[0] << "f1" << f1 << " f2"  << f2 << endl;
   eval_vect ev = 10*f1 + f2;    
   return ev; 
}

eval_vect bttc_b::max_value(int depth,const board_v& curr_board, coord &best_move,int player,int num_play) const
{
    if(depth == 0)
      {
          //final node, must evaluate
          return eval_function(curr_board,num_play);
      }
      
    list<board_v> lb;
    generate_move_state(lb,curr_board,player);
    
    eval_vect max_val(num_play);
    max_val(player) = -1000; //we focus only on current player value
    coord next_best_move(0,0);
    
    for(const board_v &b:lb)
    {
     //  cerr << "pos depl " << b.play_coord.at(0) << endl; 
       eval_vect ev = max_value(depth-1,
                               b,
                               next_best_move,
                               (player+1)%num_play,
                               num_play);
                             
        if((ev(player) > max_val(player)) || (ev(player) == max_val(player) && rand()%2 == 0) ) 
          {
                max_val = ev;
                best_move = b.play_coord[player];
              
          }
        
    }
    return max_val;
      
}

void bttc_b::generate_move_state(list<board_v> &lb,const board_v &curr_board,int player) const
{
    //0 1 2 3 for cells sure
    //4 5 6 7 for cells provisoir (+4)
    
    list<coord> lvm;
    valid_moves(lvm,curr_board.play_coord[player]);
    
    for(coord &c:lvm)
    {
       board_v tmp = curr_board;
       if(tmp(c) == 9 || tmp(c) >=4)
          tmp(c) = player+4;
          
       tmp.play_coord[player] = c;
       lb.push_back(tmp);
    }
}


void bttc_b::valid_moves(list<coord> &lvm,const coord &c) const
{
   if(c.x-1>=0) lvm.push_back(coord(c.x-1,c.y));
   if(c.x+1<35) lvm.push_back(coord(c.x+1,c.y));
   if(c.y-1>=0) lvm.push_back(coord(c.x,c.y-1));
   if(c.y+1<20) lvm.push_back(coord(c.x,c.y+1));
}

//void bttc_b::area_analysis(const coord& c,board_v& b,int &area_size,vector<list<coord> > &vlc, int num_play) const
  void bttc_b::area_analysis(const coord& c,board_v& b,int &area_size,vector<int > &vlc, int num_play) const
{


  b(c) = 11;//means visited
  list<coord>  lvm;
  valid_moves(lvm,c);

  if(lvm.size() != 4)//means we are on a border
    {
      for(int p=0;p<num_play;++p)
	{
	  if(vlc[p] > c.distance(b.play_coord[p]))
	    {
	      vlc[p] = c.distance(b.play_coord[p]);
	    }
      //	vlc[p].push_back(c); //it's a border of the area
	}
    }
  else
    {
      for(const coord& cc:lvm)
	{
	  if(b(cc) < 4) //it's a player case
	    {
	      for(int p=0;p<num_play;++p)
		{
		  //if(p == b(cc)) continue;
		  if(p != b(cc) && vlc[p] > c.distance(b.play_coord[p]))
		    {
		      vlc[p] = c.distance(b.play_coord[p]);
		    }
		  //vlc[p].push_back(c);//c, not cc
		}
	    }
	}
    }
  for(const coord& cc:lvm)
    {
      if(b(cc) == 9 )
	area_analysis(cc,b,++area_size,vlc,num_play);
    }
  
		 
}

//void bttc_b::detect_area(board_v& b, vector<pair<int, vector<list<coord>>>> &vpvlc, int num_play) const
void bttc_b::detect_area(board_v& b, vector<pair<int, vector<int>>> &vpvlc, int num_play) const
{
 
  //  int area_num = 0;
  //  vector<pair<int, vector<list<coord>>>> vpvlc(num_play);
  for(int i = 0;i<20;++i) 
    for(int j=0;j<35;++j)            
      {
	coord cur_co(j,i);
	if( b(cur_co) == 9) 
	  {
	    // vector<list<coord> > vlc(num_play);
	    vector<int > vlc(num_play,1000);
	    int area_size = 1;
	    area_analysis(cur_co,b,area_size,vlc, num_play);

	    vpvlc.push_back(make_pair(area_size,vlc));
	    //	    ++area_num;
	    //	    cerr << "area n " << area_num << " size " << area_size << endl;
	  }        
      }
}


/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    srand (time (NULL));
    int opponentCount; // Opponent count
    cin >> opponentCount; cin.ignore();

    // game loop
    while (1) {
      //vector<coord> v_play_coord(opponentCount+1,coord(0,0));

	coord v_play_coord[4]; //max 4 players
        int gameRound;
        cin >> gameRound; cin.ignore();
        int x; // Your x position
        int y; // Your y position
        int backInTimeLeft; // Remaining back in time
        cin >> x >> y >> backInTimeLeft; cin.ignore();
        
        v_play_coord[0] = coord(x,y);
      
        for (int i = 0; i < opponentCount; i++) {
            int opponentX; // X position of the opponent
            int opponentY; // Y position of the opponent
            int opponentBackInTimeLeft; // Remaining back in time of the opponent
            cin >> opponentX >> opponentY >> opponentBackInTimeLeft; cin.ignore();
            //v_play_coord.at(i+1) = coord(opponentX,opponentY);
	    v_play_coord[i+1] = coord(opponentX,opponentY);
        }
        vector<string> lines(20);
        for (int i = 0; i < 20; i++) {
            //string line; // One line of the map ('.' = free, '0' = you, otherwise the id of the opponent)
            cin >> lines.at(i); cin.ignore();
        }

        bttc_b curr_board(gameRound,
                          opponentCount,
                          backInTimeLeft,
                          v_play_coord,
                          lines);
        
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

	
       coord res = curr_board.find_next_move() ; 
       res.print_result();


    }
}
