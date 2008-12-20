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

#ifndef _SobMainWin_h_
#define _SobMainWin_h_

#include <iostream>
#include <memory>
#include <stdint.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <list>
#include <functional>

#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QDir>
#include <QColor>
#include <QTime>
#include <QBrush>

#include <QPainter>

#include "ui_sobel.h"

class SobMainWin: public QMainWindow
{
	Q_OBJECT

		public:
		SobMainWin();
		~SobMainWin();

		typedef boost::shared_array<ulong> gradarr_t;
		typedef std::pair<boost::shared_ptr<QImage>, boost::shared_ptr<QImage> >
				igrads_t; //x-grad img, y-grad img
		typedef boost::tuple<gradarr_t, gradarr_t, igrads_t> grad_t; //grad-x, grad-y, grad images

		typedef gradarr_t featarr_t;
		typedef boost::tuple<featarr_t, featarr_t, uint, uint, uint> feat_t; //  x-max[], y-max[], eye area tol. hgt,  face mid. x dist,  face mid. y dist
		//			y-max: left 1st max, l 2nd m, right 1st m, r 2nd m
		//			x-max: eye line, eye brows, hair, nose, mouth, chin
	private:
		boost::scoped_ptr<Ui::MainWindow> mwin_ui;

		boost::scoped_ptr<QImage> in_im;
		boost::scoped_ptr<QImage> out_im;
		QString fn;

		bool sobel;
		bool bin;

		void connects();

		QRgb To_gray( QRgb );
		//void Gamma( float );
		void Smooth();
		//void Binarize();

		void Pss( int, const QString & cmt = QString() );

		inline int n_rgb( int r )
		{
			int myr = r > 255 ? 255 : r;
			myr = myr < 0 ? 0 : myr;

			return myr;
		}

		inline double d2r( double d )
		{
			return (d * M_PI) / 180.0;
		}

		inline double r2d( double r )
		{
			return r * (180.0 / M_PI);
		}

	private slots:
		void Load_file( bool );
		void Save_file( bool );
		void Do_ops( bool );
		void To_gray( bool = false );
		void Avg_blur( bool );
		void Gauss_blur( bool );
		void Median_fr( bool );
		void Otsus_bin( bool );
		void Lame_bin( bool );
		void Sobel_op( bool );
		void Canny_ed( bool );
		void Hough_tm( bool );
		void Do_enables( bool );
		void Display_imgs();
		void Prep_to_extr(bool);
		boost::shared_ptr<grad_t> Make_grads( bool );
		boost::shared_ptr<feat_t> Make_feats( bool );


		void Set_gamma_lbl( int );

		void Disp_grad( bool );
		void Disp_feat( bool );
		void Crop_face( bool );

		void Do_auto( bool );

};

#endif // _SobMainWin_h_
