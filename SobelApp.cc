/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    ---
    Copyright (C) 2008, Krzysztof Kundzicz <athantor@gmail.com>
 */

#include <SobelApp.hh>

SobelApp::SobelApp(int c, char ** v) : QApplication(c, v), mwin(new SobMainWin()) /*in_im(new QImage()), out_im(0)*/
{
//     Load_file();
//     Sobel_op();
//   //  Binarize();
//     Save_file();

    mwin -> show();

}

SobelApp::~SobelApp()
{
}
/*
void SobelApp::Load_file()
{
    fn = QFileDialog::getOpenFileName(0, "Podaj obrazek", QDir::homePath(), "Obrazki (*.jpg *.jpeg *.gif *.bmp *.png)\nWsio (*)" );
    
    if(fn == "")
    {
	QMessageBox::critical(0, "FAIL", "Zly obrazek");
	this -> quit();
    } else {
	if(not (in_im -> load(fn)) )
	{
	    QMessageBox::critical(0, "FAIL", "Nie idzie wczytac");
	    this -> quit();
	} else {
	    in_im.reset(new QImage(in_im -> convertToFormat(QImage::Format_RGB32)) );
	    out_im.reset(new QImage( in_im -> size(), QImage::Format_RGB32 ) );
	    out_im -> fill( 0xff0000 );
	}
    }
    
    std::cout << in_im -> format() << std::endl;
}

QRgb SobelApp::To_gray(QRgb r)
{
    int v = QColor::fromRgb(r).toHsv().value();
    return QColor(v,v,v).rgb();
}

void SobelApp::To_gray()
{

    for(int y = 0; y < in_im -> height(); ++y)
    {
	
	for(int x = 0; x < in_im -> width(); ++x)
	{
	    //int v = QColor::fromRgb(in_im -> pixel(x,y)).toHsv().value();
	    int v = qGray(in_im -> pixel(x,y));
	    in_im -> setPixel(x,y, QColor( v, v, v ).rgb() ) ;
	}
    }

}

void SobelApp::Gamma(float f)
{
    for(int y = 0; y < in_im -> height(); ++y) //rows
    {
	for(int x = 0; x < in_im -> width(); ++x) //cols
	{
	    QRgb val = in_im -> pixel(x,y);
	    QColor nval;
	    
	    nval.setRed( 255 * std::pow( qRed(val) / 255.0, f) );
	    nval.setBlue( 255 * std::pow( qBlue(val) / 255.0, f) );
	    nval.setGreen( 255 * std::pow( qGreen(val) / 255.0, f) );
	    
	}
    }
}

void SobelApp::Smooth()
{
    uint8_t Smth[3][3] = { {1,1,1}, {1,-4,1}, {1,1,1} };
    uint8_t px = 0;
    
    for( int y = 0; y < in_im -> height(); y++)
    {
	for(int x = 0; x < in_im -> width(); x++)
	{
	    if( (y == 0) or (y+1 >=  in_im -> height()) )
	    {
		px = 0;
	    }
	    else if( (x == 0) or (x+1 >=  in_im -> width()) )
	    {
		px = 0;
	    }
	    else 
	    {
		for(int i = -1; i <= 1; i++)
		{
		    for(int j = -1; j <= 1; j++)
		    {
			px = (qRed( in_im -> pixel(x+i, y+j) ) * Smth[i+1][j+1]);
		    }
		}
	    }
	    
	    
	   out_im -> setPixel(x, y, QColor(px, px, px).rgb() );
	    
	}
    }
}

void SobelApp::Binarize()
{
    // www.icsd.aegean.gr/lecturers/kavallieratou/ICDAR2005.pdf 
    ulong s = 0;
    uchar avg = 0;
    uint8_t maxpx = 0;
    uint8_t minpx = 255;
    
    //--------------------- S1:
    
    for( int y = 0; y < out_im -> height(); y++)
    {
	for(int x = 0; x < out_im -> width(); x++)
	{
	    uint8_t px = qRed(out_im -> pixel(x,y));
	    s += px;
	    maxpx = std::max(px, maxpx);
	    minpx = std::min(px, minpx);
	}
    }
    
    avg = s / ( (in_im -> height()) * (in_im -> width()) );
    
    //----------------- S2:
    
    for( int y = 0; y < out_im -> height(); y++)
    {
	for(int x = 0; x < out_im -> width(); x++)
	{
	    uchar px = qRed(out_im -> pixel(x,y));
	    
	    int tmp = 255 - avg + px;
	    uchar dif = tmp < 0 ? 0 : tmp;
	    dif = tmp > 255 ? 255 : tmp;
	      
	    out_im -> setPixel(x,y, QColor(dif, dif, dif).rgb());
	}
    }
    
    //------------------S3:
    
    std::cout << (int) minpx << std::endl;
    
    for( int y = 0; y < out_im -> height(); y++)
    {
	for(int x = 0; x < out_im -> width(); x++)
	{
	    uchar Is = qRed(out_im -> pixel(x,y));
	    
	    int tmp = 255 - ( (255 - Is) / (255 - minpx)  ) ;
	    uchar Ii = tmp < 0 ? 0 : tmp;
	    Ii = tmp > 255 ? 255 : tmp;
	      
	    out_im -> setPixel(x,y, QColor(Ii, Ii, Ii).rgb());
	}
    }
    
}

void SobelApp::Sobel_op()
{
   To_gray();
   Smooth();
    
    Gamma(0.5);
  
    
     int8_t Gx[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1} };
     int8_t Gy[3][3] = { {1,2,1}, {0,0,0} , {-1,-2,-1} };
    
    // Scharr
//     int8_t Gx[3][3] = { {3,10,3}, {0,0,0}, {-3,-10,-3} };
//     int8_t Gy[3][3] = { {3,0,-3}, {10,0,-10} , {3,0,-3} };


    int sumx, sumy, sum = 0;

    for( int y = 0; y < in_im -> height(); y++)
    {
	for(int x = 0; x < in_im -> width(); x++)
	{
	    sumx = sumy = 0;
	    
	    if( (y == 0) or (y+1 >=  in_im -> height()) )
	    {
		sum = 0;
	    }
	    else if( (x == 0) or (x+1 >=  in_im -> width()) )
	    {
		sum = 0;
	    }
	    else 
	    {
		for(int i = -1; i <= 1; i++)
		{
		    for(int j = -1; j <= 1; j++)
		    {
			sumx += (qBlue( in_im -> pixel(x+i, y+j) ) * Gx[i+1][j+1]);
			sumy += (qBlue( in_im -> pixel(x+i, y+j) ) * Gy[i+1][j+1]);
		    }
		}
		
		if(sumx > 255) sumx = 255;
		if(sumx < 0) sumx = 0;
		
		if(sumy > 255) sumy = 255;
		if(sumy < 0) sumy = 0;
		
		sum = std::abs(sumx) + std::abs(sumy);
	    }
	    
	    sum = 255 - static_cast<uchar>( sum );
	    
	    
	    out_im -> setPixel(x, y, QColor(sum, sum, sum).rgb() );
	    
	}
    }

}

bool SobelApp::Save_file()
{

    QString ofn(QFileDialog::getSaveFileName(0, "Podaj obrazek do zapisu", QDir::homePath() + "/out-img.png" ) );
  //  in_im -> save(ofn, 0, 50);
    
    if(ofn == "")
    {
	QMessageBox::critical(0, "FAIL", "Zly obrazek");
	this -> quit();
    } else {
	if(not (out_im -> save(ofn, 0, 75 )) )
	{
	    QMessageBox::critical(0, "FAIL", "Nie idzie zapisać");
	    this -> quit();
	} 
    } 
    
    return true;
}*/