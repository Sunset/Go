/*   Author: Alan Gieske
     Project: Go
     
     Description: Assignment for programming languages course
     GoBoard builds the board and is responsible for
     executing each game step.
 */
#include "GoBoard.h"

//builds a 2d array of stone*
GoBoard::GoBoard( int width, int height ) 
{
  //set the board width and height
  this->width = width;
  this->height = height;
  emptyOwner = EMPTY;
  
  //make space for the grid
  grid = new Stone **[width];
  for( int x = 0; x<width; x++ ) 
    {
      grid[x] = new Stone *[height];
      for( int y = 0; y < height; y++ ) 
	{
	  grid[x][y] = new Stone( EMPTY, x, y );
	}
    }
}

GoBoard::~GoBoard() 
{
  for( int x = 0; x<width; x++ )
    {
    for( int y = 0; y<height; y++ ) 
      {
	delete grid[x][y]; //iterate through the board deleteing all stones
      }
    
    delete [] grid[x]; //delete each column
    }
  
  delete [] grid; //delete the grid
}

int GoBoard::step( Player player ) 
{
  Player currPlayer = player;
  Player Enemy;
  int x, y;
  Stone *currStone;
  std::string playerName;

  //set player name as string
  if( currPlayer == BLACK ) 
    {
      playerName = "Black";
    }
  else
    {
      playerName = "White";
    }
  
  std::cout << "It is now " << playerName << "\'s turn" << std::endl;
  if( currPlayer == BLACK ) 
    {
      Enemy = WHITE;
    } 
  else 
    {
      Enemy = BLACK;
    }

  //get move from user
  //ugly, ugly, code
  while( 1 )
    {
    while( 1 ) 
      {
	std::cout << "Please enter the desired column, or enter -1 to pass ";
	std::cin >> x;
	if( x == -1 )
	  {
	    return -1;
	  }
      if( x < 0 || x >= width ) 
	{
	  std::cout << "Please enter a valid column " << std::endl;
	}
      else 
	{
	  break;
	}
      }//end of nested while loop
    while( 1 ) 
      {
	std::cout << "Please enter the desired row, or enter -1 to pass " << std::endl;
	std::cin >> y;
      if( y == -1 ) 
	{
	  return -1;
	}
      if( y < 0 || y >= height ) 
	{
	  std::cout << "Please enter a valid row " << std::endl;
	} 
      else 
	{
	  break;
	}
      }//end of nested while loop
    if( grid[x][y]->getPlayer() == EMPTY )
      {
	currStone = grid[x][y];
	break;
      }
    else
      {
	std::cout << "Please enter Col/Row of an empty space." << std::endl;
      }
    }//end of the outer while loop


  
  //set piece
  currStone->setPlayer( currPlayer );

  //test and possibly kill enemies
  effectEnemies( Enemy, x, y );

  //test if player killed themselves
  if( countLiberties( currPlayer, x, y ) == 0 )
    {
      clearGroup( currPlayer, x, y );
    }
  
  //clear all our Counted marks
  clearMarks();

  //tell the main loop that the player made a move
  return 1;  
}//end of function step(Player player)

void GoBoard::effectEnemies( Player enemy, int x, int y ) 
{
  //test for enemies
  
  //array's to represent orthogonal neighbors
  int xnew[4] = { x-1, x, x+1, x };
  int ynew[4] = { y, y-1, y, y+1 };

  //iterate through each neighbor
  for( int i = 0; i < 4; i++ )
    {
      //make sure we don't go outside the board
      if( xnew[i] >= 0 && xnew[i] < width && ynew[i] >= 0 && ynew[i] < height ) 
	{
	  
	  //see if the stone is an enemy and has NOT been counted yet
	  if( grid[xnew[i]][ynew[i]]->getPlayer() == enemy && grid[xnew[i]][ynew[i]]->isCounted() == 0 )
	    {
	      //check if the group now has no liberties
	      if( countLiberties( enemy, xnew[i], ynew[i] ) == 0 ) 
		{
		  //set all stones to empty
		  clearGroup( enemy, xnew[i], ynew[i] );
		}
	    }
	}
    }//end of for loop
}//end of void effectEnemies function

int GoBoard::countLiberties( Player player, int x, int y ) 
{
  int Liberties = 0;
  
  //mark that a stone has already been counted
  grid[x][y]->setCounted();

  //array's to reresent the orthogonal neighbors
  int xnew[4] = { x-1, x, x+1, x };
  int ynew[4] = { y, y-1, y, y+1 };

  //loop through the neighbors
  for( int i = 0; i < 4; i++ )
    {
      //make certain that we don't attempt to reach outside the board
      if( xnew[i] >= 0 && xnew[i] < width && ynew[i] >= 0 && ynew[i] < height ) 
	{
	  //check if it's the same player
	  if( grid[xnew[i]][ynew[i]]->getPlayer() == player )
	    {
	      //Make sure we don't double count
	      if( grid[xnew[i]][ynew[i]]->isCounted() == 0 )
		{
		  //find that stone's liberties and add it to the total
		  Liberties += countLiberties( player, xnew[i], ynew[i] );
		} 
	    }
	  else if( grid[xnew[i]][ynew[i]]->getPlayer() == EMPTY )
	    {
	      //if empty then we found a liberty
	      Liberties += 1;
	    }	  
	}
    }//end of for loop

  //if Liberties is 0 here, the group is dead
  return Liberties;
}//end of countliberties function


void GoBoard::clearGroup( Player player, int x, int y ) 
{
  //clear the first node
  grid[x][y]->setPlayer( EMPTY );

  //array's for the orthogonal neighbors
  int xnew[4] = { x-1, x, x+1, x };
  int ynew[4] = { y, y-1, y, y+1 };

  //iterate through each neighbor
  for( int i = 0; i < 4; i++ )
    {

      //make sure we don't go outside the board
      if( xnew[i] >= 0 && xnew[i] < width && ynew[i] >= 0 && ynew[i] < height ) 
	{
	  //make sure the stone is of the correct player
	  if( grid[xnew[i]][ynew[i]]->getPlayer() == player )
	    {
	      //clear the stone and any stones of the same player attached to it
	      clearGroup( player, xnew[i], ynew[i] );
	    }
	}
    }//end of for loop
}//end of clearGroup function

void GoBoard::clearMarks()
{

  //clear all the counted marks on the board
  for( int x = 0; x < width; x++ ) 
    {
      for( int y = 0; y < height; y++ )
	{
	  grid[x][y]->clearCounted();
	}
    }
}

Player GoBoard::scoreArea()
{
  int emptySize;
  int scoreBlack = 0;
  int scoreWhite = 0;
  for( int x = 0; x < width; x++ )
    {
      for( int y = 0; y < height; y++ )
	{
	  if( grid[x][y]->isCounted() == 0 )
	    {
	      switch( grid[x][y]->getPlayer() )
		{
		case( BLACK ):

		  //find the size of the black group
		  scoreBlack += getSize( BLACK, x, y );
		  break;
		case( WHITE ):

		  //find the size of the white group
		  scoreWhite += getSize( WHITE, x, y );
		  break;
		case( EMPTY ):
		  emptySize = 0;

		  //find the size of the empty area
		  emptySize = scoreEmpty( x, y );
		  
		  //figure out the owner of the controlled territory
		  //add it to their score
		  switch( emptyOwner )
		    {
		    case( BLACK ):
		      scoreBlack += emptySize;
		      break;
		    case( WHITE ):
		      scoreWhite += emptySize;
		      break;
		    default:
		      break;
		    }     //end of nested switch statement
		  emptyOwner = EMPTY;
		  break;
		default:
		  break;
		}//end of outer switch statement
	      //set the grid piece as counted
	      grid[x][y]->setCounted();
	    }//end of if statement
	}//end of for loop
    }//end of for loop

  //find the bigger number
  if( scoreWhite > scoreBlack )
    {
      return WHITE;
    }
  else if( scoreBlack > scoreWhite )
    {
      return BLACK;
    }
  else
    {
      return EMPTY;
    }
}//end of scoreArea function

int GoBoard::scoreEmpty( int x, int y )
{
  //don't double count
  grid[x][y]->setCounted();
  int size = 0;

  //array's for the orthogonal neighbors
  int xnew[4] = { x-1, x, x+1, x };
  int ynew[4] = { y, y-1, y, y+1 };

  //iterate through each neighbor
  for( int i = 0; i < 4; i++ )
    {

      //make sure we don't go outside the board
      if( xnew[i] >= 0 && xnew[i] < width && ynew[i] >= 0 && ynew[i] < height ) 
	{
	  //Don't double count
	  if( grid[xnew[i]][ynew[i]]->isCounted() == 0 )
	    {
	      //if neighbor is empty keep counting
	      if( grid[xnew[i]][ynew[i]]->getPlayer() == EMPTY )
		{
		  size += scoreEmpty( xnew[i], ynew[i] );
		}
	      //if we haven't run into a player owned piece yet, or we run into
	      //the same players piece, that player still owns the group
	      else if( grid[xnew[i]][ynew[i]]->getPlayer() == emptyOwner || emptyOwner == EMPTY )
		{
		  emptyOwner = (Player)grid[xnew[i]][ynew[i]]->getPlayer();
		}
	      else
		{
		  //set the empty group to dead,
		  //no one gets the points
		  emptyOwner = DOMI;
		}
	    }//end of if statement
	}//end of if statemenmt
    }//end of for loop

  //return the size plus one for the stone we're on
  return size + 1;
}//end of scoreEmpty function

int GoBoard::getSize( Player player, int x, int y )
{
  //don't double count
  grid[x][y]->setCounted();
  int size = 0;

  //array's for the orthogonal neighbors
  int xnew[4] = { x-1, x, x+1, x };
  int ynew[4] = { y, y-1, y, y+1 };

  //iterate through each neighbor
  for( int i = 0; i < 4; i++ )
    {

      //make sure we don't go outside the board
      if( xnew[i] >= 0 && xnew[i] < width && ynew[i] >= 0 && ynew[i] < height ) 
	{
	  //Check if it's the same player and that we haven't double counted
	  if( grid[xnew[i]][ynew[i]]->getPlayer() == player && grid[xnew[i]][ynew[i]]->isCounted() == 0 )
	    {
	      size += getSize( player, xnew[i], ynew[i] );
	    }
	}
    }//end of for loop

  return size + 1;
}//end of getSize function

void GoBoard::printboard() 
{
  std::cout << "  0 1 2 3 4 5 6 7 8 9" << std::endl;
  for( int y = 0; y<height; y++ ) 
    {
      std::cout << y;

      for( int x = 0; x<width; x++ ) 
	{
	  std::cout << " " << grid[x][y]->getPlayer();
	}

      std::cout << std::endl;
    }
}
	  
  
