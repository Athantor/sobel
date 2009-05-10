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

#include "SobMainWin.hh"

SobMainWin::SobMainWin() :
	mwin_ui(new Ui::MainWindow()), sobel(false), bin(false), XTOLPCT(5.0), YTOLPCT(7.5)
{
	mwin_ui -> setupUi(this);

	mwin_ui -> label -> setText("<b style='font-size: 72pt;'>x</b>");
	mwin_ui -> label_2 -> setText("<b style='font-size: 72pt;'>x</b>");

	kropuj = 0;

	connects();
}

SobMainWin::~SobMainWin()
{
}

void SobMainWin::connects()
{
	connect(mwin_ui -> actionWczytaj, SIGNAL(triggered ( bool ) ), this, SLOT(Load_file(bool)));
	connect(mwin_ui -> actionZapisz, SIGNAL(triggered ( bool ) ), this, SLOT(Save_file(bool)));
	connect(mwin_ui -> actionDajesz, SIGNAL(triggered ( bool ) ), this, SLOT(Sobel_op(bool)));
	connect(mwin_ui -> actionSzaro, SIGNAL(triggered ( bool ) ), this, SLOT(To_gray(bool)));
	connect(mwin_ui -> actionGradient, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_grad(bool)));
	connect(mwin_ui -> actionAvg_blur, SIGNAL(triggered ( bool ) ), this, SLOT(Avg_blur(bool)));
	connect(mwin_ui -> actionGauss, SIGNAL(triggered ( bool ) ), this, SLOT(Gauss_blur(bool)));
	connect(mwin_ui -> actionFiltr_medianowy, SIGNAL(triggered ( bool ) ), this, SLOT(Median_fr(bool)));
	connect(mwin_ui -> actionBinaryzacja_2, SIGNAL(triggered ( bool ) ), this, SLOT(Otsus_bin(bool)));
	connect(mwin_ui -> actionLame, SIGNAL(triggered ( bool ) ), this, SLOT(Lame_bin(bool)));
	connect(mwin_ui -> actionHough, SIGNAL(triggered ( bool ) ), this, SLOT(Hough_tm(bool)));
	connect(mwin_ui -> actionCanny, SIGNAL(triggered ( bool ) ), this, SLOT(Canny_ed(bool)));

	connect(mwin_ui -> actionGo, SIGNAL(triggered ( bool ) ), this, SLOT(Do_auto(bool)));

	connect(mwin_ui -> actionSegmentacja_koloru, SIGNAL(triggered ( bool ) ), this, SLOT(Face_find_cs(bool)));

	connect(mwin_ui -> actionMaska_gradient_w, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_feat(bool)));
	connect(mwin_ui -> actionOczy_houghem, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_eyes_ht(bool)));
	connect(mwin_ui -> actionOczy_sobelem, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_eyes_sob(bool)));
	connect(mwin_ui -> actionOtsuem, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_eyes_otsu(bool)));

	// connect(mwin_ui -> actionPrzytnij, SIGNAL(triggered ( bool ) ), this, SLOT(Crop_face(bool)) );
	//connect(mwin_ui -> actionPrzytnij, SIGNAL(triggered ( bool ) ), this, SLOT(Crop_face_manual(bool)));

	connect(mwin_ui -> verticalSlider, SIGNAL( sliderMoved ( int ) ), this, SLOT(Set_gamma_lbl(int)));


}

void SobMainWin::Do_ops( bool )
{

	statusBar() -> showMessage(QString::fromUtf8("Liczę sobla…"), 0);

	QTime t;
	t.start();
	Sobel_op(0);
	statusBar() -> showMessage(QString::number(t.elapsed()) + "ms", 5000);

	Display_imgs();
}

void SobMainWin::Display_imgs()
{
	this -> setCursor(Qt::WaitCursor);

	mwin_ui -> label -> setPixmap(QPixmap::fromImage(in_im -> scaled(mwin_ui -> label -> size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
	mwin_ui -> label_2 -> setPixmap(QPixmap::fromImage(out_im -> scaled(mwin_ui -> label -> size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));

	this -> setCursor(Qt::ArrowCursor);
}

void SobMainWin::Do_enables( bool e )
{
	mwin_ui -> actionDajesz -> setEnabled(e);
	mwin_ui -> actionZapisz -> setEnabled(e);
	mwin_ui -> actionSzaro -> setEnabled(e);
	mwin_ui -> actionGradient -> setEnabled(e);
	mwin_ui -> actionBinaryzacja_2 -> setEnabled(e);
	mwin_ui -> actionLame -> setEnabled(e);
	mwin_ui -> actionAvg_blur -> setEnabled(e);
	mwin_ui -> actionGauss -> setEnabled(e);
	mwin_ui -> actionFiltr_medianowy -> setEnabled(e);
	mwin_ui -> actionHough -> setEnabled(e);
	mwin_ui -> actionCanny -> setEnabled(e);

	//mwin_ui -> verticalSlider -> setEnabled(e);
	mwin_ui -> checkBox -> setEnabled(e);
	mwin_ui -> checkBox_2 -> setEnabled(e);

	//	mwin_ui -> actionGo -> setEnabled(e);

	mwin_ui -> actionMaska_gradient_w -> setEnabled(e);
	mwin_ui -> actionOczy_houghem -> setEnabled(e);
	mwin_ui -> actionOtsuem -> setEnabled(e);
	//mwin_ui -> actionPrzytnij -> setEnabled(e);
	mwin_ui -> actionSegmentacja_koloru -> setEnabled(e);

}

void SobMainWin::Load_file( bool )
{
	fn = QFileDialog::getOpenFileName(0, "Podaj obrazek", QDir::homePath(), "Obrazki (*.jpg *.jpeg *.gif *.bmp *.png *.pgm)\nWsio (*)");

	if ( fn == "" ) {
		QMessageBox::critical(0, "FAIL", "Nie podano obrazka");
		Do_enables(0);

		return;
	}
	else {
		in_im.reset(new QImage());
		this -> setCursor(Qt::WaitCursor);
		if ( not (in_im -> load(fn)) ) {
			this -> setCursor(Qt::ArrowCursor);
			QMessageBox::critical(0, "FAIL", "Nie idzie wczytac");
			Do_enables(0);

			in_im.reset(0);
			return;
		}
		else {
			in_im.reset(new QImage(in_im -> convertToFormat(QImage::Format_RGB32)));
			out_im.reset(new QImage(*in_im));
			//  out_im -> fill( 0xffffffff );

			Do_enables(1);

			bin = false;
			sobel = false;

			Display_imgs();
			this -> setCursor(Qt::ArrowCursor);

		}
	}

	statusBar() -> showMessage(fn + " [fmt:" + QString::number(in_im -> format()) + "]", 5000);
	setWindowTitle(fn + " [" + QString::number(in_im -> width()) + QString::fromUtf8("×") + QString::number(in_im -> height()) + "]");

	//     std::cout << in_im -> format() << std::endl;
}

void SobMainWin::Save_file( bool )
{
	QString ofn(QFileDialog::getSaveFileName(0, "Podaj obrazek do zapisu", QDir::homePath() + "/out-img.png"));
	//  in_im -> save(ofn, 0, 50);

	QImage myimg;

	if ( sobel and (not bin) ) {
		myimg = out_im -> convertToFormat(QImage::Format_Indexed8);
	}

	if ( bin or (not sobel) ) {
		myimg = out_im -> convertToFormat(QImage::Format_Mono);
	}

	if ( ofn == "" ) {
		QMessageBox::critical(0, "FAIL", "Zly obrazek");
		return;
	}
	else {
		this -> setCursor(Qt::WaitCursor);
		if ( not (myimg . save(ofn, "png", 50)) ) {
			this -> setCursor(Qt::ArrowCursor);
			QMessageBox::critical(0, "FAIL", "Nie idzie zapisać");
			return;
		}

		this -> setCursor(Qt::ArrowCursor);
	}

	statusBar() -> showMessage(QString::fromUtf8("„%1” → „%2”").arg(fn).arg(ofn), 5000);
}

void SobMainWin::Set_gamma_lbl( int v )
{
	float gamma = v / 10.0;
	mwin_ui -> label_3 -> setText(QString::number(gamma));

	//	Gamma(v);
	Display_imgs();

}

void SobMainWin::Pss( int t, const QString & cmt )
{
	if ( cmt.isEmpty() ) {
		statusBar() -> showMessage(QString::fromUtf8("%1ms [%2px/ms]").arg(t).arg(std::ceil((out_im -> width() * out_im -> height()) / (t * 1.0))), 5000);
	}
	else {
		statusBar() -> showMessage(
				QString::fromUtf8("%1ms [%2px/ms] | %3").arg(t).arg(std::ceil((out_im -> width() * out_im -> height()) / (t * 1.0))).arg(cmt), 5000);
	}
}

void SobMainWin::Disp_grad( bool )
{
	this -> setCursor(Qt::WaitCursor);
	boost::scoped_ptr<QDialog> wgt(new QDialog(this, Qt::Dialog));
	boost::scoped_ptr<QLabel> imgl(new QLabel(wgt.get()));
	boost::scoped_ptr<QLabel> xgdtl(new QLabel(wgt.get()));
	boost::scoped_ptr<QLabel> ygdtl(new QLabel(wgt.get()));
	boost::scoped_ptr<QPixmap> xgdtli(new QPixmap(in_im -> size()));
	boost::scoped_ptr<QPixmap> ygdtli(new QPixmap(in_im -> size()));

	boost::shared_ptr<grad_t> g(Make_grads(0));

	wgt -> setMinimumSize(640, 480);
	wgt -> setMaximumSize(640, 480);
	wgt -> setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	imgl -> resize(490, 330);
	imgl -> move(0, 0);

	xgdtli -> fill(QColor("white"));
	ygdtli -> fill(QColor("white"));

	//-------------


	const int maxx = std::max_element(g -> get<0> ().get(), g -> get<0> ().get() + out_im -> height()) - g -> get<0> ().get();
	const int maxy = std::max_element(g -> get<1> ().get(), g -> get<1> ().get() + out_im -> width()) - g -> get<1> ().get();
	long mx = g -> get<0> ()[maxx];
	long my = g -> get<1> ()[maxy];

	QPainter gx(xgdtli.get()), gy(ygdtli.get());
	/**/
	gx.setPen("black");
	for ( int i = 1; i < xgdtli -> height(); ++i ) {
		gx.drawLine(0, i - 1, static_cast<int> ((g -> get<0> ())[i] / (mx * 1.0) * xgdtli -> width()), i);
	}
	//     gx.setPen("red");
	//     gx.drawRect(0, g -> get<2>() - 5, xgdtli -> width(), g -> get<2>() + 5);

	gx.end();

	gy.setPen("black");
	for ( int i = 1; i < ygdtli -> width(); ++i ) {
		//std::cout << "*" ;
		gy.drawLine(i - 1, 0, i, static_cast<int> ((g -> get<1> ())[i] / (my * 1.0) * ygdtli -> height()));
	}

	gy.end();

	//---------

	QImage tmp(*out_im);
	/*	QPainter qp(&tmp);

	 qp.setPen("red");
	 qp.setBrush(QBrush(QColor("red")));
	 qp.setOpacity(0.05);
	 qp.drawRect(0, g -> get<2> ()[0] - (g -> get<5> ()), tmp.width(), g -> get<
	 5> () * 2);
	 qp.setOpacity(1);

	 qp.setPen(QPen(QColor("red"), 1.5, Qt::SolidLine));
	 qp.drawLine(0, g -> get<2> ()[0], tmp.width(), g -> get<2> ()[0]);

	 qp.setPen(QPen(QColor("red"), 1.5, Qt::DashLine));
	 qp.drawLine(0, g -> get<2> ()[1], tmp.width(), g -> get<2> ()[1]);
	 qp.drawLine(0, g -> get<2> ()[3], tmp.width(), g -> get<2> ()[3]);
	 qp.drawLine(0, g -> get<2> ()[4], tmp.width(), g -> get<2> ()[4]);
	 qp.setPen(QPen(QColor("red"), 1.5, Qt::DotLine));
	 qp.drawLine(0, g -> get<2> ()[2], tmp.width(), g -> get<2> ()[2]);
	 qp.drawLine(0, g -> get<2> ()[5], tmp.width(), g -> get<2> ()[5]);

	 qp.setPen(QPen(QColor("blue"), 1.5, Qt::SolidLine));
	 qp.drawLine(g -> get<3> ()[0], 0, g -> get<3> ()[0], tmp.height());
	 qp.drawLine(g -> get<3> ()[2], 0, g -> get<3> ()[2], tmp.height());

	 qp.setPen(QPen(QColor("blue"), 1.5, Qt::DotLine));
	 qp.drawLine(g -> get<3> ()[1], 0, g -> get<3> ()[1], tmp.height());
	 qp.drawLine(g -> get<3> ()[3], 0, g -> get<3> ()[3], tmp.height());

	 qp.setPen(QPen(QColor("green"), 1.5, Qt::DashLine));
	 qp.drawLine( g -> get<6> () , 0, g -> get<6> () , tmp.height());

	 */
	imgl -> setPixmap(QPixmap::fromImage(tmp.scaled(imgl -> size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));

	//--------

	xgdtl -> resize(150, imgl -> pixmap() -> height());
	xgdtl -> move(imgl -> pixmap() -> width(), 0);

	ygdtl -> resize(imgl -> pixmap() -> width(), 150);
	ygdtl -> move(0, imgl -> pixmap() -> height());

	xgdtl -> setPixmap(xgdtli -> scaled(xgdtl -> size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	ygdtl -> setPixmap(ygdtli -> scaled(ygdtl -> size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

	this -> setCursor(Qt::ArrowCursor);
	wgt -> exec();

}

void SobMainWin::Prep_to_extr( bool d, uint m, bool g, bool s )
{
	To_gray(d);
	if ( g )
		Gauss_blur(d);
	if ( s )
		Sobel_op(d);
	Avg_blur(d);
	Otsus_bin(d);
	for ( uint i = 0; i < m; i++ ) {
		Median_fr(d);
	}
}

void SobMainWin::Disp_eyes_sob( bool en )
{

}

void SobMainWin::Do_auto( bool )
{

	Prep_to_extr(true);
	//Disp_grad(true);
	Disp_feat(false);
}

void SobMainWin::Crop_face( bool d )
{
	QImage tmp(*in_im);

	Prep_to_extr(true);

	boost::shared_ptr<feat_t> g = Make_feats(true);

	out_im.reset(new QImage(tmp.copy(g->get<1> ()[1], g->get<0> ()[2], g -> get<1> ()[3] - g -> get<1> ()[1], g -> get<0> ()[5] - g -> get<0> ()[2])));
	if ( !d )
		Display_imgs();

}

void SobMainWin::Disp_feat( bool d )
{
	QImage tmp(*out_im);

	Prep_to_extr(true);

	boost::shared_ptr<feat_t> g = Make_feats(true);

	QPainter qp(&tmp);

	qp.setPen("red");
	qp.setBrush(QBrush(QColor("red")));
	qp.setOpacity(0.05);
	qp.drawRect(0, g -> get<0> ()[0] - (g -> get<2> ()), tmp.width(), g -> get<2> () * 2);
	qp.setOpacity(1);

	qp.setPen(QPen(QColor("red"), 1.5, Qt::SolidLine));
	qp.drawLine(0, g -> get<0> ()[0], tmp.width(), g -> get<0> ()[0]);

	qp.setPen(QPen(QColor("red"), 1.5, Qt::DashLine));
	qp.drawLine(0, g -> get<0> ()[1], tmp.width(), g -> get<0> ()[1]);
	qp.drawLine(0, g -> get<0> ()[3], tmp.width(), g -> get<0> ()[3]);
	qp.drawLine(0, g -> get<0> ()[4], tmp.width(), g -> get<0> ()[4]);
	qp.setPen(QPen(QColor("red"), 1.5, Qt::DotLine));
	qp.drawLine(0, g -> get<0> ()[2], tmp.width(), g -> get<0> ()[2]);
	qp.drawLine(0, g -> get<0> ()[5], tmp.width(), g -> get<0> ()[5]);

	qp.setPen(QPen(QColor("blue"), 1.5, Qt::SolidLine));
	qp.drawLine(g -> get<1> ()[0], 0, g -> get<1> ()[0], tmp.height());
	qp.drawLine(g -> get<1> ()[2], 0, g -> get<1> ()[2], tmp.height());

	qp.setPen(QPen(QColor("blue"), 1.5, Qt::DotLine));
	qp.drawLine(g -> get<1> ()[1], 0, g -> get<1> ()[1], tmp.height());
	qp.drawLine(g -> get<1> ()[3], 0, g -> get<1> ()[3], tmp.height());

	qp.setPen(QPen(QColor("green"), 1.5, Qt::DashLine));
	qp.drawLine(g -> get<3> (), 0, g -> get<3> (), tmp.height());
	qp.drawLine(0, g -> get<4> (), tmp.width(), g -> get<4> ());

	qp.end();

	out_im.reset(new QImage(tmp));
	if ( !d )
		Display_imgs();
}

void SobMainWin::Disp_eyes_ht( bool d )
{
	boost::shared_ptr<eyeloc_t> eyes = Find_iris_ht(true);

	QImage tmp(*out_im);
	QPainter qp(&tmp);
	qp.setPen("red");
	QFont qf("monospace");
	qf.setPixelSize(static_cast<int> (eyes -> get<3> ()));
	qp.setFont(qf);

	qp.drawText(eyes -> get<0> (), "X");
	qp.drawText(eyes -> get<1> (), "X");

	qp.setPen("magenta");
	qp.drawText(eyes -> get<4> (), "+");
	qp.drawText(eyes -> get<5> (), "+");

	qp.setPen("cyan");

	qp.drawText((eyes -> get<0> () + eyes -> get<4> ()) / 2, "#");
	qp.drawText((eyes -> get<1> () + eyes -> get<5> ()) / 2, "#");

	out_im.reset(new QImage(tmp));
	if ( !d )
		Display_imgs();
}

/*void SobMainWin::Crop_face_manual( bool )
{
	kropuj = 1;
	mwin_ui -> label -> setCursor(Qt::CrossCursor);
	mwin_ui -> label -> resize(mwin_ui -> label_2 -> pixmap() -> size());
	//mwin_ui -> label_2 -> setPixmap(QPixmap::fromImage(out_im -> scaled(mwin_ui -> label -> size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}*/

/*void SobMainWin::mouseReleaseEvent( QMouseEvent * evt )
{

	static QPoint p1, p2;
	static QImage tmp;

	if ( childAt(evt->pos()) and childAt(evt->pos()) == mwin_ui->label ) {
		if ( kropuj == 1 ) {
			p1 = evt->globalPos();
			kropuj = 2;

			tmp = *in_im;

			QPainter qp(in_im.get());
			qp.setPen("red");
			QFont qf("monospace");
			qf.setPixelSize(32);
			qp.setFont(qf);

			qp.drawText(p1, "X");

			Display_imgs();

		}

		else if ( kropuj == 2 ) {
			p2 = evt->globalPos();
			kropuj = 0;

			mwin_ui -> label -> setCursor(Qt::ArrowCursor);

			in_im.reset( new QImage(tmp. copy(QRect(p1,p2)) . scaled(mwin_ui -> label -> size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)) );
			Display_imgs();

		}
	}
}*/
