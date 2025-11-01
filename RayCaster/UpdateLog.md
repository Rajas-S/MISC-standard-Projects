# Update Log    
1.11.25 - finally fixed the strange collisioncheck error. The error was with xhold,yhold - I set them after    
          I call move() but I don't initialise them so I am actually checking a line from the player to near the    
          top corner (undefeined values so undefined behaviour). I also made some changes to how I was setting the    
          x,y positions of the objects - for the player I set this->x to x*cellSide and same for y and that makes    
          everything much cleaner and more efficient and also allowed easier logic and debug tracking.  
          The method that I use the check for collsions isn't very good so I need to fix it to make it more accurate.  
