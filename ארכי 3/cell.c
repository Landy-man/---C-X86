
#define alive 0
#define dead 1

extern char state[];
extern int WorldLength;
extern int WorldWidth;

int getLocation(int x, int y){
	if(x == -1) x = WorldLength -1;
	if(x == WorldLength) x = 0;
	if(y == WorldWidth) y = 0;
	if(y == -1) y = WorldWidth - 1;
	return x*WorldWidth + y;
}

char incAlive(char cellState){
	cellState++;
	if(cellState > '9')
		cellState = '9';
	return cellState;
}






char cell(int x,int y){
	int dxy[] = {-1,0,1};
	int i,j;
	int xtmp,ytmp;
	char ans = '0';

	int count[] = {0,0};
	for(i = 0; i < 3; i++){
		xtmp = x + dxy[i];
		for(j = 0; j < 3; j++){
			if((i==1) && (j==1)) continue;
			ytmp = y + dxy[j];
			if((state[getLocation(xtmp,ytmp)]) == '0')
				count[dead] ++;
			else
				count[alive]++;
		}
	}
	char cellState = state[ getLocation(x,y) ];

	if( cellState == '0' ){
		if(count[alive] == 3)
			ans = '1';
	}else{
		if(count[alive] == 2 || count[alive] == 3)
			ans = incAlive(cellState);
		
	}

	return ans;
}


