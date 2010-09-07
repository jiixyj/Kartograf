#include "Level.h"
#include "resource.h"

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>

#include <string>
#include <vector>
#include <stack>
#include <list>
#include <iostream>
#include <fstream>
#include "global.h"

#include <QtGui>

using namespace std;

//My changes:
//ListFiles rewrited to posix mode
//gui is destroy [oh god]
//only console
//what the hell is wchar_t??
//by Firemark :)

int CWorld = 0;

int cut = 0;
bool RENDER = false;
bool water = true;

bool ListFiles(string path, vector<string>& files,int &numfiles) {
    stack<string> directories;
	DIR *dir;
	dirent *ent; 
	string temp_str;
	string temp;
	
    directories.push(path);
    files.clear();
	
	cout << "1)Generate tree of files..." << endl;
    while ( !directories.empty() ) {
		path = directories.top();
		directories.pop();
       
		dir = opendir( path.c_str() ); 

        if (!dir) return false;
		
		
        while((ent = readdir(dir)) != NULL) 
        { 
			temp_str = ent->d_name;
			
			if ( temp_str != "." && temp_str != ".." ){
				temp = path + "/" + temp_str;
				//cout << temp << " ";
				if (ent->d_type == DT_DIR ){
					directories.push(temp);
				}
				if (ent->d_type == DT_REG ){
					files.push_back(temp);
					numfiles++;
				}
					
			}
        } 
        closedir(dir);
    }
    cout << "Done!" << endl;

    return true;
};

void Dostuff() {

	string A = getenv("HOME");
	A += "/.minecraft/saves/World";
	string name = "map";
	string png = ".png";
	
	if(CWorld == 1){
		A += "1";
		name += "1";
	}
	if(CWorld == 2){
		A += "2";
		name += "2";
	}
	if(CWorld == 3){
		A += "3";
		name += "3";
	}
	if(CWorld == 4){
		A += "4";
		name += "4";
	}
	if(CWorld == 5){
		A += "5";
		name += "5";
	}
	
	cout << A << " " << name << endl;
	//return 0;
 
	// string C = L"C:\\Users\\Harm\\Documents\\Visual Studio 2008\\Projects\\WinCartograph\\test\\edit";
	
	//wcout << A.c_str();
	//return 0;
    vector<string> files;
	list<render> renderblocks;

	int counter = 0;
	int cc = 0;
	//int lool = 0;
	
	ListFiles(A, files,counter);

	//lool = files.size();
	Level foo;
	//std::ofstream myfile ("files.txt",std::ios::app);

	//decompress_one_file("C:\\Users\\Harm\\AppData\\Roaming\\.minecraft\\saves\\World3\\4\\1c\\c.4.-g.dat","level_in");
	//std::cout << "\nRendering chunks..";
	
	//if (ListFiles(C.c_str(), L"*", files,counter)){
	cout << "2)Unzip and draw..." << endl;
        for (vector<string>::iterator it = files.begin(); it != files.end(); it++) {
			 
			 const render * temp = foo.LoadLevelFromFile(it->c_str(),slide,water,cut);
			 if(temp->isgood){
				cc++;
				renderblocks.push_back(*temp);
			 }
			//}

        }
    cout << "Done!" << endl;
	//}
		//int lool2 = renderblocks.size();
	
	int minx = 100000000;
	int miny = 100000000;
	int maxx = -100000000;
	int maxy = -100000000;
	for (list<render>::iterator pit=renderblocks.begin();pit!=renderblocks.end();pit++){
		//std::cout << "  " << it->x << " " << it->y;
		if(abs(pit->x) < 65536 && abs(pit->y) < 65536){
			
			if(pit->x < minx)
			minx = pit->x;
		if(pit->y < miny)
			miny = pit->y;
		
		if(pit->x > maxx)
			maxx = pit->x;
		if(pit->y > maxy)
			maxy = pit->y;
		
		}

	}
	//std::cout << "\n\nMAXIMUM X = " << maxx << " MAXIMUM Y = " << maxy;
	//std::cout << "\n\nMINIMUM X = " << minx << " MINIMUM Y = " << miny;
	int imageheight,imagewidth;
	


	if(slide == 70){
		int cxx = ((((maxx-minx))+(maxy-miny)))*0.5;
		int cyy = (((maxx-minx))+(maxy-miny))*0.5;

	  imageheight = (cxx)*32 + 288;
	  imagewidth = (cyy)*32 + 288;
	  
		}else{
	imageheight = (maxx-minx)*16 + 144;
	imagewidth = (maxy-miny)*16 + 144;
	}
	IMG * MAP;
	if(slide == 70)
	MAP = new IMG(imageheight,imagewidth+128);
	else if(slide == 69)
	MAP = new IMG(imageheight,imagewidth);
	else
	MAP = new IMG(imageheight,imagewidth);

	//std::cout << "\nAssembling image...";
	//if(slide == 69){
	renderblocks.sort();
	renderblocks.reverse();
	//}

	//int * xytt;
	
	//xytt = new int[10000]; 
	//for(int b = 0;b < 10000;b++){
	//xytt[b] = 0;
	//}
	for (list<render>::iterator rit=renderblocks.begin();rit!=renderblocks.end();rit++){
	
		//total++;	
		int xb = rit->x;
		int yb = rit->y;

		int xx = (xb-minx)*16;
		int yy = (yb-miny)*16;

		if(slide == 70){
		if(flip == 1)
		yy = abs((yb-miny)-(maxy-miny))*16;
		if(Rotate == 0)
		xx = abs((xb-minx)-(maxx-minx))*16;
		
		

		int temx = xx;
		int temy = yy;
		xx = imageheight/2+(temx)-(temy);
		yy = (temx)+(temy)+128;

	}

	if(slide == 69){
	
		if(flip == 1){
			if(Rotate == 0){
			//xx = (xb-minx)*16;
			yy = abs((yb-miny)-(maxy-miny))*16;
			}else{
			xx = (yb-miny)*16;
			yy = abs((xb-minx)-(maxx-minx))*16;

			}
		}else{
		if(Rotate == 1){
		xx = (yb-miny)*16;
		yy = (xb-minx)*16;
		}
		}
		}
	//xytt[(xx/16)+(yy/16)*100]++;
	//if(xytt[(xx/16)+(yy/16)*100] > 1)
	//myfile << "X:" << xb << " Y:" << yb << " " << "xx:" << xx << " yy:" << yy << "  " << xytt[(xx/16)+(yy/16)*100] << endl;
	//std::cout << xx << "_" << yy << " ";
	if(xx >= 0 && yy >= 0 && xx < 40000 && yy < 40000){
		if(slide == 70){

		
	for(int xo = 0;xo < 33;xo++){
	for(int yo = 0;yo < 160;yo++){
		int x = xo;
		int y = yo;
	MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
	}}


		}else{
	for(int xo = 0;xo < 16;xo++){
	for(int yo = 0;yo < 16+(slide > 68)*128;yo++){					
		int x = xo;
		int y = yo;
		
		//MAP.SetPixel(x+xx,y+yy,sf::Color(125,255,0,255));

		if(slide == 69)
		MAP->SetPixel(x+xx,y+yy,Blend(MAP->GetPixel(x+xx,y+yy),rit->Q->GetPixel(x,y),128));
		else if(slide == 70)
		MAP->SetPixel(x+xx,y+yy-128,Blend(MAP->GetPixel(x+xx,y+yy-128),rit->Q->GetPixel(x,y),128));
		else
		MAP->SetPixel(x+xx,y+yy,rit->Q->GetPixel(x,y));
		
	}
	}}
	}}
	

	
	string txtname = "./"+name+".txt";

	//std::cout << "\n\nERRORS: " << errors << "\nTOTAL: " << total; 
	switch(slide){
		case(-1):
			name.append("-normal");
			if(!water)name.append("-nowater");
		break;
		case(-3):
			name.append("-heightcolor");
			if(!water)name.append("-nowater");
		break;
		case(-4):
			name.append("-heightgray");
			if(!water)name.append("-nowater");
		break;
		case(-5):
			name.append("-ore");
		break;
		case(69):
			name.append("-oblique");
			if(!water)name.append("-nowater");
		break;
		case(70):
			name.append("-obliqueangle");
			if(!water)name.append("-nowater");
		break;
		case(64):
		{
		std::string s;
		std::stringstream out;
		out << cut;
		s = out.str();
			name.append("-slice-");
			name += s;
		}
		break;


	}
	//if(flip)
	//name.append("-flipped");
	//if(Rotate)
	//name.append("-rotated");

	if(slide == -1 || slide > 68){
	if(flip)
	name.append("-flipped");
	if(Rotate)
	name.append("-rotated");
	if(cave)
		name.append("-cave");
	

	
	switch(daynight){
	case(0):
		name.append("-day");
	break;
	case(1):
		name.append("-day/night");
	break;
	case(2):
		name.append("-night");
	break;
	}
	}

	if(exclude != 0){
		name.append("-only-");
		std::string s;
		std::stringstream out;
		out << exclude;
		s = out.str();
		name += s;

	}
	
	


	string exit ="./" + name + png;

	cout << "3)Save image" << endl;
	QImage Output(reinterpret_cast<uchar*>(MAP->d),imageheight,imagewidth,QImage::Format_ARGB32);
        Output.save(QString::fromStdString(exit));
	
	delete MAP;
	//delete MAP;

	//remove("level_in");
	renderblocks.clear();
	cout << "4)Save txt" << endl;

	ofstream values( txtname.c_str() );
	if (values.is_open())
	{
		values << "Map surface is: " << cc*16*16 << " square meters";
		values << "end1";
		values << "Block amounts:";
		values << "end1";
		values << "Air: " << foo.count[0] << "end1";
		values << "Stone: " << foo.count[1] << "end1";
		values << "Grass: " << foo.count[2] << "end1";
		values << "Dirt: " << foo.count[3] << "end1";
		values << "Snow: " << foo.count[78] << "end1";
		values << "end1";
		values << "Water: " << foo.count[8]+foo.count[9] << "end1";
		values << "Ice: " << foo.count[79] << "end1";
		values << "Lava: " <<	foo.count[10]+foo.count[11] << "end1";
		values << "end1";
		values << "Obisidian: " <<	foo.count[49] << "end1";
		values << "end1";
		values << "Trunk: " << foo.count[17] << "end1";
		values << "Leaves: " << foo.count[18] << "end1";
		values << "Wood: " << foo.count[5] << "end1";
		values << "Cactus: " << foo.count[81] << "end1";
		values << "end1";
		values << "Sand: " << foo.count[12] << "end1";
		values << "Gravel: " << foo.count[13] << "end1";
		values << "Clay: " << foo.count[82] << "end1";
		values << "end1";
		values << "Gold Ore: " << foo.count[14] << "end1";
		values << "Iron Ore: " << foo.count[15] << "end1";
		values << "Coal Ore: " << foo.count[16] << "end1";
		values << "Diamond Ore: " << foo.count[56] << "end1";
		values << "Redstone: " << foo.count[73]+foo.count[74] << "end1";
		values << "end1";
		values << "Cobble: " << foo.count[4] << "end1";
		values << "Glass: " << foo.count[20] << "end1";
		values << "Cloth: " << foo.count[35] << "end1";
		values << "Gold: " << foo.count[41] << "end1";
		values << "Iron: " << foo.count[42] << "end1";
		values << "Diamond: " << foo.count[57] << "end1";
		values << "end1";
		values << "Farmland: " << foo.count[60] << "end1";
		values << "Crops: " << foo.count[59] << "end1";
		values << "Reed: " << foo.count[83] << "end1";
		values << "Torch: " << foo.count[50] << "end1";
		values << "CraftTable: " << foo.count[58] << "end1";
		values << "Chest: " << foo.count[54] << "end1";
		values << "Furnace: " << foo.count[61]+foo.count[62] << "end1";
		values << "Wooden Doors: " << foo.count[64]/2 << "end1";
		values << "Iron Doors: " << foo.count[71] << "end1";
		values << "Signs: " << foo.count[63]+foo.count[68] << "end1";
		values << "Ladder: " << foo.count[65] << "end1";
		values << "Railtracks: " << foo.count[66] << "end1";
		values << "Wooden Stairs: " << foo.count[53] << "end1";
		values << "Rock Stairs: " << foo.count[67] << "end1";
		values << "Lever: " << foo.count[69] << "end1";
		values << "Buttons: " << foo.count[77] << "end1";
		values << "Pressure Plates: " << foo.count[70]+foo.count[72] << "end1";
		values << "Redstone Powder: " << foo.count[55] << "end1";
		values << "Redstone Torches: " << foo.count[75]+foo.count[76] << "end1";
		
		values << endl;
	

	
	values.close();
	}else cout << "fatal write to txt" << endl;
/*
	for (list<render>::iterator it=renderblocks.begin();it!=renderblocks.end();++it){
	for(int x = 0;x < 16;x++){
			for(int y = 0;y < 16;y++){
				std::cout << " " << (int)it->data[x+y*16];
			}}
			int p;
			std::cin >> p;
		}
*/
	
   // return 0;
}

int main(){
	
	string flag = "";
	
	cout << "Cartography rewritten for linux by Firemark [pozdrawiam halp]" << std::endl;
	
	while (CWorld>5||CWorld<1){
		cout << "select world:[1,2,3,4,5]" << endl << ":" << flush;
		cin >> CWorld;
	}
	
		slide = -1;
		cout << "Select flags:" << endl
		<< "W - water; C - cave mode" << endl
		<< "R - rotate; F - flip  " << endl
		<< "D - day; d -day/night; N - night " << endl
		<< "Hc - heightcolor; Hg - heightgray; Or - Ore " << endl
		<< "Ob - oblique; Oa - oblique angle" << endl
		<< "E - exclude " << endl << ":" << flush;
		
		cin >> flag;
	
		if ( flag.find("W") != string::npos )
			water = true;
		else 
			water = false;
		
		if ( flag.find("C") != string::npos )
			cave = true;
		else 
			cave = false;
		
		if ( flag.find("R") != string::npos )
			Rotate = true;
		else 
			Rotate = false;
		
		if ( flag.find("F") != string::npos )
			flip = true;
		else 
			flip = false;
		
		if ( flag.find("D") != string::npos )
			daynight = 0;
		if ( flag.find("d") != string::npos )
			daynight = 1;
		if ( flag.find("n") != string::npos )
			daynight = 2;
		
		if ( flag.find("Hc") != string::npos )
			slide = -3;
		if ( flag.find("Hg") != string::npos )
			slide = -4;
		if ( flag.find("Or") != string::npos )
			slide = -5;
		if ( flag.find("Ob") != string::npos )
			slide = 69;//yeah funny
		if ( flag.find("Oa") != string::npos )
			slide = 70;
		if ( flag.find("E") != string::npos ){
				cout << "Write number to exclude [0-128]" << endl << ":" << flush;
				cin >> exclude;
		}
			
		
		
	Dostuff();
	
	return 0;
};
	

