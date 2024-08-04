#include <iostream>
#include <ctime>
#include <windows*h>
#define Closed_Cell 0
#define Exploded_Mine 1
#define Unchecked_Cell 2
#define Opened_Cell 3
#define Flag 4
#define Free_Cell 0
#define Mine 9
using namespace std;
char **gen_field(char **field,int a,int b,int bombs,boolean autoplay){
	int i,j,used_space=0,skip=0;
	field=new char*[a];
	for(i=0;i<a;i++){
		field[i]=new char[b];
		for(j=0;j<b;j++)
			field[i][j]=Free_Cell;
	}
	for(i=0;i<a;i++){
		for(j=0;j<b;j++){
			if(bombs<=0){
				break;
			}
			if(field[i][j]==Mine){
				skip++;
				continue;
			}
			if(rand()%((a*b)-(i*b)-j-used_space+skip)==0){
				field[i][j]=Mine;
				bombs--;
				used_space++;
				skip=i=0;
				j=-1;
			}
		}
	}
	for(i=0;i<a;i++){
		for(j=0;j<b;j++){
			if(field[i][j]>8){
				if(i-1>=0)
					field[i-1][j]++;
				if(i+1<a)
					field[i+1][j]++;
				if(j-1>=0)
					field[i][j-1]++;
				if(j+1<b)
					field[i][j+1]++;
				if(i-1>=0&&j-1>=0)
					field[i-1][j-1]++;
				if(i-1>=0&&j+1<b)
					field[i-1][j+1]++;
				if(i+1<a&&j-1>=0)
					field[i+1][j-1]++;
				if(i+1<a&&j+1<b)
					field[i+1][j+1]++;
			}
		}
	}
	for(i=0;i<a;i++){
		for(j=0;j<b;j++){
			if(field[i][j]>8)
				field[i][j]=Mine;
		}
	}
	cout<<endl;
	cout<<"field is generated\n\n";
	return field;
}
int check_near_cell(char **field,char **player_view,int check,int i,int j){
	if(field[i][j]==Free_Cell && player_view[i][j]==Closed_Cell){
		player_view[i][j]=Unchecked_Cell;
		check++;
	}
	if(field[i][j]>0 && field[i][j]<9 && player_view[i][j]==Closed_Cell){
		player_view[i][j]=Opened_Cell;
	}
	return check;
}
void open_space(char **field,char **player_view,int a,int b,int x,int y){
	int i,j,check=0,xDiv2=x/2;
	if(!(x%2)){
		if(y<a && xDiv2<b){
			if(player_view[y][xDiv2]==Closed_Cell && field[y][xDiv2]>0 && field[y][xDiv2]<9){
				player_view[y][xDiv2]=Opened_Cell;
			}
			if(player_view[y][xDiv2]==Closed_Cell && field[y][xDiv2]==Free_Cell){
				player_view[y][xDiv2]=Unchecked_Cell;
				check++;
				for(i=0;i<a;i++){
					for(j=0;j<b;j++){
						if(player_view[i][j]==Unchecked_Cell){
							player_view[i][j]=Opened_Cell;
							check--;
							if(i-1>=0){
								check=check_near_cell(field,player_view,check,i-1,j);
							}
							if(i-1>=0&&j-1>=0){
								check=check_near_cell(field,player_view,check,i-1,j-1);
							}
							if(j-1>=0){
								check=check_near_cell(field,player_view,check,i,j-1);
							}
							if(i+1<a&&j-1>=0){
								check=check_near_cell(field,player_view,check,i+1,j-1);
							}
							if(i+1<a){
								check=check_near_cell(field,player_view,check,i+1,j);
							}
							if(i+1<a&&j+1<b){
								check=check_near_cell(field,player_view,check,i+1,j+1);
							}
							if(j+1<b){
								check=check_near_cell(field,player_view,check,i,j+1);
							}
							if(i-1>=0&&j+1<b){
								check=check_near_cell(field,player_view,check,i-1,j+1);
							}
							if(check>0)
								i=0;
								j=-1;
						}
					}
				}
			}
			if(field[y][xDiv2]==Mine && player_view[y][xDiv2]!=Flag)
				player_view[y][xDiv2]=Exploded_Mine;
		}
	}
}
void play(char **field,int a,int b,int bombs,boolean autoplay){
	HANDLE hIn,hOut;
	INPUT_RECORD InRec;
	DWORD NumRead;
	CONSOLE_CURSOR_INFO Curs;
	CONSOLE_SCREEN_BUFFER_INFO Buff;
	COORD zero={0,0};
	boolean game_over=false,win=false;
	int flag=0,clean=2,x,y,i,j,input=1,marked_bombs=0,opened_cells=0,bombs_at_beginning=bombs,xDiv2;
	char **player_view=new char*[a];
	for(i=0;i<a;i++){
		player_view[i]=new char[b];
		for(j=0;j<b;j++)
			player_view[i][j]=Closed_Cell; //0-закрытое поле, 3-открытое поле
	}
	for(i=bombs;(i/=10)>0;)
		clean++;
	system("cls");
	hIn=GetStdHandle(STD_INPUT_HANDLE);
	hOut=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hOut,&Curs);
	SetConsoleMode(hIn,ENABLE_MOUSE_INPUT|ENABLE_EXTENDED_FLAGS);
	while(!game_over){
		Curs*bVisible=false;
		SetConsoleCursorInfo(hOut,&Curs);
		SetConsoleCursorPosition(hOut,zero);
		for(i=0;i<a;i++){
			for(j=0;j<b;j++){
				switch(player_view[i][j]){
					case Closed_Cell:{
						SetConsoleTextAttribute(hOut,(WORD)(0|8));
						WriteConsoleW(hOut,L"■ ",wcslen(L"■ "),NULL,NULL);
						SetConsoleTextAttribute(hOut,(WORD)(0|15));
						break;
					}
					case Exploded_Mine:{
						SetConsoleTextAttribute(hOut,(WORD)(0|4));
						WriteConsoleW(hOut,L"☼ ",wcslen(L"☼ "),NULL,NULL);
						SetConsoleTextAttribute(hOut,(WORD)(0|15));
						game_over=true;
						break;
					}
					case Opened_Cell:{
						opened_cells++;
						switch(field[i][j]){
							case Free_Cell:{
								SetConsoleTextAttribute(hOut,(WORD)(0|8));
								WriteConsoleW(hOut,L"□ ",wcslen(L"□ "),NULL,NULL);
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 1:{
								SetConsoleTextAttribute(hOut,(WORD)(0|9));
								cout<<"1 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 2:{
								SetConsoleTextAttribute(hOut,(WORD)(0|10));
								cout<<"2 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 3:{
								SetConsoleTextAttribute(hOut,(WORD)(0|12));
								cout<<"3 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 4:{
								SetConsoleTextAttribute(hOut,(WORD)(0|5));
								cout<<"4 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 5:{
								SetConsoleTextAttribute(hOut,(WORD)(0|4));
								cout<<"5 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 6:{
								SetConsoleTextAttribute(hOut,(WORD)(0|1));
								cout<<"6 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 7:{
								SetConsoleTextAttribute(hOut,(WORD)(0|2));
								cout<<"7 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case 8:{
								SetConsoleTextAttribute(hOut,(WORD)(0|8));
								cout<<"8 ";
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								break;
							}
							case Mine:{
								SetConsoleTextAttribute(hOut,(WORD)(0|15));
								WriteConsoleW(hOut,L"☼ ",wcslen(L"☼ "),NULL,NULL);
							}
						}
						break;
					}
					case Flag:{
						SetConsoleTextAttribute(hOut,(WORD)(0|12));
						WriteConsoleW(hOut,L"⚐ ",wcslen(L"⚐ "),NULL,NULL);
						SetConsoleTextAttribute(hOut,(WORD)(0|15));
						if(field[i][j]==9)
							marked_bombs++;
						else
							marked_bombs--;
					}
				}
			}
			cout<<endl;
		}
		if(marked_bombs==bombs_at_beginning || opened_cells>=(a*b-bombs_at_beginning))
			win=game_over=true;
		opened_cells=0;
		marked_bombs=0;
		cout<<"\nMines left ";
		for(i=clean;i!=0;i--)
			cout<<' ';
		GetConsoleScreenBufferInfo(hOut,&Buff);
		Buff*dwCursorPosition*X-=clean;
		SetConsoleCursorPosition(hOut,Buff*dwCursorPosition);
		cout<<bombs;
		cout<<"\ntype 0 to exit\n>";
		Curs*bVisible=true;
		SetConsoleCursorInfo(hOut,&Curs);
		if(!autoplay){
			ReadConsoleInput(hIn,&InRec,1,&NumRead);
			switch(InRec*EventType){
				case MOUSE_EVENT:{
					if(InRec*Event*MouseEvent*dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED){
						x=InRec*Event*MouseEvent*dwMousePosition*X;
						y=InRec*Event*MouseEvent*dwMousePosition*Y;
						open_space(field,player_view,a,b,x,y);
					}
					if(InRec*Event*MouseEvent*dwButtonState==RIGHTMOST_BUTTON_PRESSED){
						x=InRec*Event*MouseEvent*dwMousePosition*X;
						y=InRec*Event*MouseEvent*dwMousePosition*Y;
						xDiv2=x/2;
						if(y<a && xDiv2<b && !(x%2)){
							if(player_view[y][xDiv2]==Closed_Cell)
								flag=1;
							if(player_view[y][xDiv2]==Flag)
								flag=2;
							if(flag==1){
								player_view[y][xDiv2]=Flag;
								bombs--;
							}
							if(flag==2){
								player_view[y][xDiv2]=Closed_Cell;
								bombs++;
							}
							flag=0;
						}
					}
					break;
				}
				case KEY_EVENT:{
					if(InRec*Event*KeyEvent*uChar*AsciiChar=='0'){
						game_over=true;
						cout<<"0\n";
					}
				}
			}
		}else{
			y=rand()%a;
			x=rand()%(2*b);
			open_space(field,player_view,a,b,x,y);
		}
	}
	if(game_over){
		GetConsoleScreenBufferInfo(hOut,&Buff);
		Buff*dwCursorPosition*X--;
		Buff*dwCursorPosition*Y--;
		SetConsoleCursorPosition(hOut,Buff*dwCursorPosition);
		if(!win){
			SetConsoleTextAttribute(hOut,(WORD)(0|4));
			cout<<"GAME OVER     \n";
		}
		if(win){
			SetConsoleTextAttribute(hOut,(WORD)(0|10));
			cout<<"YOU WIN       \n";
		}
		SetConsoleTextAttribute(hOut,(WORD)(0|15));
		cout<<"Press any button to exit***";
		do{
			ReadConsoleInput(hIn,&InRec,1,&NumRead);
		} while(InRec*EventType!=KEY_EVENT);
	}
	system("cls");
}
int main(){
	srand(time(NULL));
	int input,a,b,x,bombs;
	char **field=new char*;
	boolean field_was_generated=false;
	do{
		cout<<"chose option\n1*generate field\n2*play\n3*creators\n4*autotest\n0*exit\n>";
		cin>>input;
		cout<<'\n';
		switch(input){
			case 1:{
				cout<<"chose one of presets, or create custom\n1*easy(9x9 10b)\n2*medium(16x16 40b)\n3*expert(16x30 99b)\n4*create custom\n0*go back\n>";
				cin>>x;
				cout<<'\n';
				switch(x){
					case 1:{
						field=gen_field(field,9,9,10,false);
						a=b=9;
						bombs=10;
						field_was_generated=true;
						continue;
					}
					case 2:{
						field=gen_field(field,16,16,40,false);
						a=b=16;
						bombs=40;
						field_was_generated=true;
						continue;
					}
					case 3:{
						field=gen_field(field,16,30,99,false);
						a=16;
						b=30;
						bombs=99;
						field_was_generated=true;
						continue;
					}
					case 4:{
						cout<<"type height:";
						cin>>a;
						if(a>48)
							a=48;
						cout<<"type wight:";
						cin>>b;
						if(b>105)
							b=105;
						cout<<"number of bombs:";
						cin>>bombs;
						if(bombs>a*b) 
							bombs=a*b;
						field=gen_field(field,a,b,bombs,false);
						field_was_generated=true;
						continue;
					}
					default:{
						x=0;
						continue;
					}
				}
			}
			case 2:{
				if(field_was_generated){
					play(field,a,b,bombs,false);
					cout<<"GG\n\n";
				}else{
					cout<<"generate field at first\n\n";
				}
				break;
			}
			case 3:{
				cout<<"creators:\nLakshman C(22PT08)\nAnirudhan R(22PT02)\n\n";
				break;
			}
			case 4:{
				a=rand()%48+1;
				b=rand()%105+1;
				bombs=rand()%(a*b);
				field=gen_field(field,a,b,bombs,true);
				play(field,a,b,bombs,true);
				break;
			}
			default:input=0;
		}
	}while(input!=0);
}
