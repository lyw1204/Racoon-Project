

//Global variable for depth map accesses
short depthBaseline [Y_RES][X_RES];
short depthNow [Y_RES][X_RES]; 
short processing [Y_RES - 2][X_RES - 2]; //Temporary matrix



//Outputs 2D matrices for nice printing
void printMatrix(short matrix[Y_RES][X_RES]) {
  for (int i = 0; i < Y_RES; i++) {
    for (int j = 0; j < X_RES; j++) {
      Serial.print(matrix[i][j]);
      Serial.print(' ');
    }
    Serial.print('\n');
  }
}

//Filtering functions

void diffMatrix(short matrix1[Y_RES][X_RES], short matrix2 [Y_RES][X_RES]) { //In place matrix difference done on matrix 2
  for (int i = 0; i < Y_RES; i++) {
    for (int j = 0; j < X_RES; j++) {
      matrix2[i][j] -= matrix1[i][j];
    }
  }
}


short median9 (short arr[9]) { //Bubble sort and return middle
  short temp = 0;
  for (int j = 0; j < 9; j++) {
    for (int i = 0; i < 8; i++) {

      if (arr[i] > arr[i + 1]) {
        temp = arr[i];
        arr[i] = arr[i + 1];
        arr[i + 1] = temp;
      }
    }
  }
  return arr[4];
}
void medianFilter(short matrix[Y_RES][X_RES]) {//Applies median filter on matrix
  short neighbors [9];

  for (int i = 0; i < Y_RES - 2; i++) { //Populate processing matrix using medians of 9 neighbors
    for (int j = 0; j < X_RES - 2; j++) {
      //Populates 9 neighbors
      neighbors[0] = matrix[i][j];
      neighbors[1] = matrix[i + 1][j];
      neighbors[2] = matrix[i + 2][j];
      neighbors[3] = matrix[i][j + 1];
      neighbors[4] = matrix[i + 1][j + 1];
      neighbors[5] = matrix[i + 2][j + 1];
      neighbors[6] = matrix[i][j + 2];
      neighbors[7] = matrix[i + 1][j + 2];
      neighbors[8] = matrix[i + 2][j + 2];

      processing[i][j] = median9(neighbors);

    }
  }
  //Edge pixels will remain unchanged
  for (int i = 1; i < Y_RES - 1; i++) {
    for (int j = 1; j < X_RES - 1; j++) { //Change all pixels but the outer edge
      matrix[i][j] = processing[i - 1][j - 1];
    }

  }
}


bool judgeMatrix(short matrix[Y_RES][X_RES]){//Returns true if human, else false
  short colAccum[Y_RES-2];
  char topRow = 0;
  char botRow = 0;
  
  for(short i = 1; i<Y_RES-1; i++){
    colAccum[i-1] = 0;//Initializes colAccum array elements
    
    for(short j = 1; j<X_RES-1; j++){
      if (matrix[i][j]<-10){//Count less-than-zero pixels for every row and store in colAccum
        colAccum[i-1] ++;        
        }
      }
    if(colAccum[i-1]>4){//Write 1 if more than 4 pixels are true for that line, else write 0
      colAccum[i-1] = 1;
      }
    else{
      colAccum[i-1] = 0;
      }
    Serial.println(colAccum[i-1]);
    }

  for(char i = 0; i<Y_RES-2; i++){
    if(colAccum[i]){
      topRow = i;
      break;
      }
    }
   for(char i = Y_RES-2-1; i>=0; i--){

    if(colAccum[i]){
      botRow = i;
      break;
      }
    } 

   if(abs(botRow-topRow) >= 3){
    return true;
    }//Human
   else{
    return false; 
    }//Raccoon
  }
