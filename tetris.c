#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	createRankList();
	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_REC_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawNextBlock(nextBlock);
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;

	if(check_recommend){
		if(root) free(root);
		root = (TNode*) malloc(sizeof(TNode));
		root->level = 0;
		for(int i = 0; i < HEIGHT; i++){
			for(int j = 0; j < WIDTH; j++){
				root->recField[i][j] = field[i][j];
			}
		}
			
		root->accumulatedScore = modified_recommend(root);
	}

	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){	
		move(10+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				if((blockY + i >= HEIGHT || blockY + i < 0) || (blockX + j >= WIDTH || blockX + j < 0) || f[blockY + i][blockX + j] == 1) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	
	int nx = blockX, ny = blockY, nr = blockRotate;
	switch(command){
		case KEY_UP:
			if(blockRotate == 0) nr = 3;
			else nr = blockRotate - 1;
			break;
		case KEY_DOWN:
			ny--;
			break;
		case KEY_RIGHT:
			nx--;
			break;
		case KEY_LEFT:
			nx++;
			break;
		default:
			break;
	}
	int y = blockY;
	for(; y < HEIGHT; y++){
		if(!(CheckToMove(f, currentBlock, nr, y + 1, nx))){
			break;
		}
	}
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(block[currentBlock][nr][i][j] == 1){
				move(i+ny+1,j+nx+1);
				printw("%c",'.');
				move(i+y+1,j+nx+1);
				printw("%c",'.');
			}
		}
	}
	
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);

	// move(blockY, blockX);

}

void BlockDown(int sig){
	// user code
	timed_out = 0;
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)){
		blockY++;
		DrawChange(field, KEY_DOWN ,nextBlock[0], blockRotate, blockY, blockX);
	}else{
		if(blockY == -1){
			gameOver = 1;
		}else{
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand() % 7;
			blockRotate=0;
			blockY=-1;
			blockX=WIDTH/2-2;
			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
		}
	}
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int touched = 0;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				f[blockY + i][blockX + j] = 1;
				if(blockY + i == HEIGHT - 1 || f[blockY + i + 1][blockX + j] == 1) touched++;
			}
		}
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int res = 0;
	for(int i = 0; i < HEIGHT; i++){
		int cnt = 0;
		for(int j = 0; j < WIDTH; j++){
			if(f[i][j] == 1) cnt++;
		}
		if(cnt == WIDTH){
			res++;
			for(int ii = i; ii >= 0; ii--){
				for(int jj = 0; jj < WIDTH; jj++){
					if(ii != 0){ f[ii][jj] = f[ii - 1][jj];}
					else f[ii][jj] = 0;
				}
			}
			i--;
		}
	}
	return res * res * 100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	for(; y < HEIGHT; y++){
		if(!(CheckToMove(field, blockID, blockRotate, y + 1, x))){
			break;
		}
	}
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	if(check_recommend) DrawRecommend(root->recBlockY, root->recBlockX, root->curBlockID, root->recBlockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
}

void createRankList(){
	
	FILE *fp;
	int i, j, score;
	char name[NAMELEN + 1];
	fp = fopen("rank.txt", "r");
	
	head = malloc(sizeof(NODE));
	head->next = NULL;
	if (fscanf(fp, "%d", &score_number) != EOF) {
		NODE* node = head;
		for(i = 0; i < score_number; i++){
			fscanf(fp, "%s %d", name, &score);
			NODE* next = malloc(sizeof(NODE));
			strcpy(next->user, name);
			next->score = score;
			next->next = NULL;
			node->next = next;
			node = next;
		}

	}
	else {
		score_number = 0;
	}
	fclose(fp);
}

void rank(){
	int X=1, Y=score_number, ch, i, j;
	clear();

	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	ch = wgetch(stdscr);

	if (ch == '1') {
		echo();
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);

		if(Y > score_number || X > Y || X < 1){
			printw("search failure: no rank in the list\n");
		}else{
			printw("       name       |   score   \n");
			printw("------------------------------\n");
			NODE* node = head;
			for (int i = 0; i < X; i++){
				node = node->next;
			}
			for (int i = X; i <= Y; i++) {
				printw(" %-17s| %-10d\n", node->user, node->score);
				node = node->next;
			}
		}
		noecho();
	}

	else if ( ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		echo();
		printw("Input the name: ");
		scanw("%s", str);
		NODE* node = head;
		while(node != NULL){
			if(strcmp(str, node->user) == 0){
				check = 1;
			}
			node = node->next;
		}
		if(!check){
			printw("search failure: no name in the list\n");
		}else{
			printw("       name       |   score   \n");
			printw("------------------------------\n");
			node = head;
			while(node != NULL){
				if(strcmp(str, node->user) == 0){
					printw(" %-17s| %-10d\n", node->user, node->score);
				}
				node = node->next;
			}
		}
		noecho();
	}

	else if ( ch == '3') {
		int num;
		echo();
		printw("Input the rank: ");
		scanw("%d", &num);
		
		if(num < 1 || num > score_number){
			printw("\nsearch failure: the rank not in the list\n");
		}else{
			NODE* node = head;
			for(int i = 0; i < num - 1; i++) node = node->next;

			NODE *tmp;
			tmp = node->next;
			node->next = tmp->next;
			score_number--;
			free(tmp);
			writeRankFile();
			printw("\nresult: the rank deleted\n");
		}

	}
	getch();

}

void writeRankFile(){
	int sn, i;
	FILE *fp = fopen("rank.txt", "w");

	fprintf(fp, "%d\n", score_number);
	NODE *node = head->next;  
    while (node) {
        fprintf(fp, "%s %d\n", node->user, node->score);
        node = node->next;
    }

	fclose(fp);
}

void newRank(int score){
	char str[NAMELEN+1];
	int i, j;
	clear();
	echo();
	printw("your name: ");
	scanw("%s", str);
	noecho();
	NODE* node = malloc(sizeof(NODE));
	node->score = score;
	strcpy(node->user, str);
	node->next = NULL;

	NODE* curnode = head;
	while(curnode->next != NULL && curnode->next->score > score){
		curnode = curnode->next;
	}
	if(curnode->next == NULL) {
		curnode->next = node;
	}
	else {
		node->next = curnode->next;
		curnode->next = node;
	}
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(TNode *root){
	int max=0; 
	int y, x, score;
	root->child = (TNode**) malloc(sizeof(TNode*) * (CHILDREN_MAX + 1)); 
	root->accumulatedScore = -1000;
	int depth = root->level;
	root->curBlockID = nextBlock[depth];
	root->recBlockY = -1;

	for(int r = 0; r < 4; r++){
		for(int i = 0; i <= WIDTH; i++){
			x = i - 2;
			if(CheckToMove(root->recField, root->curBlockID, r, 0, x)){
				root->child[i] = (TNode*) malloc(sizeof(TNode));
				score = 0;
				y = 0;
				while (CheckToMove(root->recField, root->curBlockID, r, y + 1, x)) y++;
				
				for(int j = 0; j < HEIGHT; j++){
					for(int k = 0; k < WIDTH; k++){
						root->child[i]->recField[j][k] = root->recField[j][k];
					}
				}
				score += AddBlockToField(root->child[i]->recField, root->curBlockID, r, y, x);
				score += DeleteLine(root->child[i]->recField);

				if(root->level + 1 < BLOCK_NUM){
					root->child[i]->level = root->level + 1;
					score += recommend(root->child[i]);;
				}else root->child[i]->level = root->level + 1;

				if(score > root->accumulatedScore || (score == root->accumulatedScore && root->recBlockY < y)) {
					root->accumulatedScore = score;
					root->recBlockY = y;
					root->recBlockX = x;
					root->recBlockRotate = r;
					max = score;
				}
				free(root->child[i]);
			}
		}	
	}


	return max;
}
int modified_recommend(TNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	// user code
	int y, x, score, w = 5;
	root->child = (TNode**) malloc(sizeof(TNode*) * (CHILDREN_MAX + 1)); 
	root->accumulatedScore = -1000;
	int depth = root->level;
	root->curBlockID = nextBlock[depth];
	root->recBlockY = -1;

	for(int r = 0; r < 4; r++){
		for(int i = 0; i <= WIDTH; i++){
			x = i - 2;
			if(CheckToMove(root->recField, root->curBlockID, r, 0, x)){
				root->child[i] = (TNode*) malloc(sizeof(TNode));
				score = 0;
				y = 0;
				while (CheckToMove(root->recField, root->curBlockID, r, y + 1, x)) y++;
				
				for(int j = 0; j < HEIGHT; j++){
					for(int k = 0; k < WIDTH; k++){
						root->child[i]->recField[j][k] = root->recField[j][k];
					}
				}
				score += AddBlockToField(root->child[i]->recField, root->curBlockID, r, y, x);
				score += DeleteLine(root->child[i]->recField);
				score += y * w;
				if(root->level + 1 < BLOCK_NUM){
					root->child[i]->level = root->level + 1;
					score += modified_recommend(root->child[i]);;
				}else root->child[i]->level = root->level + 1;

				// 점수의 최댓값을 구한다.
				if(score > root->accumulatedScore || (score == root->accumulatedScore && root->recBlockY < y)) {
					root->accumulatedScore = score;
					root->recBlockY = y;
					root->recBlockX = x;
					root->recBlockRotate = r;
					max = score;
				}
				free(root->child[i]);
			}
		}	
	}


	return max;
}

void recommendedPlay(){
    TNode *root;
    check_recommend = 1;           
    clear();                        
    act.sa_handler = BlockDown;
    sigaction(SIGALRM, &act, &oact);
    InitTetris();                 
    refresh();
    napms(300);

    while(!gameOver){
        root = (TNode*)malloc(sizeof(TNode));
        root->level = 0;
        memcpy(root->recField, field, sizeof(field));
        root->accumulatedScore = modified_recommend(root);

        int targetX      = root->recBlockX;
        int targetRotate = root->recBlockRotate;
        free(root);
		if (blockY < 0) {
			ProcessCommand(KEY_DOWN);
			refresh(); napms(100);
    	}

        for(int r = 0; r < targetRotate; ++r){
            ProcessCommand(KEY_UP);
            refresh(); napms(200);
        }

        while(blockX < targetX){
            ProcessCommand(KEY_RIGHT);
            refresh(); napms(150);
        }
        while(blockX > targetX){
            ProcessCommand(KEY_LEFT);
            refresh(); napms(150);
        }

        while(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){
            ProcessCommand(KEY_DOWN);
            refresh(); napms(100);
        }
        BlockDown(0);      
        refresh(); napms(300);
    }
}

    

