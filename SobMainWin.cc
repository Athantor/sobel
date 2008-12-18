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
	mwin_ui(new Ui::MainWindow()), sobel(false), bin(false)
{
	mwin_ui -> setupUi(this);

	mwin_ui -> label -> setText("<b style='font-size: 72pt;'>x</b>");
	mwin_ui -> label_2 -> setText("<b style='font-size: 72pt;'>x</b>");

	connects();
}

SobMainWin::~SobMainWin()
{
}

void SobMainWin::connects()
{
	connect(mwin_ui -> actionWczytaj, SIGNAL(triggered ( bool ) ), this, SLOT(Load_file(bool)) );
	connect(mwin_ui -> actionZapisz, SIGNAL(triggered ( bool ) ), this, SLOT(Save_file(bool)) );
	connect(mwin_ui -> actionDajesz, SIGNAL(triggered ( bool ) ), this, SLOT(Sobel_op(bool)) );
	connect(mwin_ui -> actionSzaro, SIGNAL(triggered ( bool ) ), this, SLOT(To_gray(bool)) );
	connect(mwin_ui -> actionGradient, SIGNAL(triggered ( bool ) ), this, SLOT(Disp_grad(bool)) );
	connect(mwin_ui -> actionAvg_blur, SIGNAL(triggered ( bool ) ), this, SLOT(Avg_blur(bool)) );
	connect(mwin_ui -> actionGauss, SIGNAL(triggered ( bool ) ), this, SLOT(Gauss_blur(bool)) );
	connect(mwin_ui -> actionFiltr_medianowy, SIGNAL(triggered ( bool ) ), this, SLOT(Median_fr(bool)) );
	connect(mwin_ui -> actionBinaryzacja_2, SIGNAL(triggered ( bool ) ), this, SLOT(Otsus_bin(bool)) );
	connect(mwin_ui -> actionLame, SIGNAL(triggered ( bool ) ), this, SLOT(Lame_bin(bool)) );
	connect(mwin_ui -> actionHough, SIGNAL(triggered ( bool ) ), this, SLOT(Hough_tm(bool)) );
	connect(mwin_ui -> actionCanny, SIGNAL(triggered ( bool ) ), this, SLOT(Canny_ed(bool)) );

	connect(mwin_ui -> verticalSlider, SIGNAL( sliderMoved ( int ) ), this, SLOT(Set_gamma_lbl(int)) );
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
	mwin_ui -> label -> setPixmap(QPixmap::fromImage(in_im -> scaled(
			mwin_ui -> label -> size(), Qt::KeepAspectRatio,
			Qt::SmoothTransformation)));
	mwin_ui -> label_2 -> setPixmap(QPixmap::fromImage(out_im -> scaled(
			mwin_ui -> label -> size(), Qt::KeepAspectRatio,
			Qt::SmoothTransformation)));
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
}

void SobMainWin::Load_file( bool )
{
	fn = QFileDialog::getOpenFileName(0, "Podaj obrazek", QDir::homePath(),
			"Obrazki (*.jpg *.jpeg *.gif *.bmp *.png *.pgm)\nWsio (*)");

	if(fn == "")
	{
		QMessageBox::critical(0, "FAIL", "Nie podano obrazka");
		Do_enables(0);

		return;
	}
	else
	{
		in_im.reset(new QImage());
		if(not (in_im -> load(fn)))
		{
			QMessageBox::critical(0, "FAIL", "Nie idzie wczytac");
			Do_enables(0);

			in_im.reset(0);
			return;
		}
		else
		{
			in_im.reset(new QImage(in_im -> convertToFormat(
					QImage::Format_RGB32)));
			out_im.reset(new QImage(*in_im));
			//  out_im -> fill( 0xffffffff );

			Do_enables(1);

			bin = false;
			sobel = false;

			Display_imgs();

		}
	}

	statusBar() -> showMessage(fn + " [fmt:" + QString::number(
			in_im -> format()) + "]", 5000);
	setWindowTitle(fn + " [" + QString::number(in_im -> width())
			+ QString::fromUtf8("×") + QString::number(in_im -> height()) + "]");

	//     std::cout << in_im -> format() << std::endl;
}

void SobMainWin::Save_file( bool )
{
	QString ofn(QFileDialog::getSaveFileName(0, "Podaj obrazek do zapisu",
			QDir::homePath() + "/out-img.png"));
	//  in_im -> save(ofn, 0, 50);

	QImage myimg;

	if(sobel and (not bin))
	{
		myimg = out_im -> convertToFormat(QImage::Format_Indexed8);
	}

	if(bin or (not sobel))
	{
		myimg = out_im -> convertToFormat(QImage::Format_Mono);
	}

	if(ofn == "")
	{
		QMessageBox::critical(0, "FAIL", "Zly obrazek");
		return;
	}
	else
	{
		if(not (myimg . save(ofn, "png", 50)))
		{
			QMessageBox::critical(0, "FAIL", "Nie idzie zapisać");
			return;
		}
	}

	statusBar() -> showMessage(
			QString::fromUtf8("„%1” → „%2”").arg(fn).arg(ofn), 5000);
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
	if(cmt.isEmpty())
	{
		statusBar() -> showMessage(
				QString::fromUtf8("%1ms [%2px/ms]").arg(t).arg(std::ceil(
						(out_im -> width() * out_im -> height()) / (t * 1.0))),
				5000);
	}
	else
	{
		statusBar() -> showMessage(
				QString::fromUtf8("%1ms [%2px/ms] | %3").arg(t).arg(std::ceil(
						(out_im -> width() * out_im -> height()) / (t * 1.0))).arg(
						cmt), 5000);
	}
}

void SobMainWin::Disp_grad( bool )
{
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

	long mx = g -> get<0> ()[g -> get<2> ()[0]];
	long my = g -> get<1> ()[g -> get<3> ()[0]];

	QPainter gx(xgdtli.get()), gy(ygdtli.get());
	/**/
	gx.setPen("black");
	for(int i = 1; i < xgdtli -> height(); ++i)
	{
		gx.drawLine(0, i - 1, static_cast<int> ((g -> get<0> ())[i]
				/ (mx * 1.0) * xgdtli -> width()), i);
	}
	//     gx.setPen("red");
	//     gx.drawRect(0, g -> get<2>() - 5, xgdtli -> width(), g -> get<2>() + 5);

	gx.end();

	gy.setPen("black");
	for(int i = 1; i < ygdtli -> width(); ++i)
	{
		//std::cout << "*" ;
		gy.drawLine(i - 1, 0, i, static_cast<int> ((g -> get<1> ())[i] / (my
				* 1.0) * ygdtli -> height()));
	}

	gy.end();

	//---------

	QImage tmp(*out_im);
	QPainter qp(&tmp);

	qp.setPen("red");
	qp.setBrush(QBrush(QColor("red")));
	qp.setOpacity(0.2);
	qp.drawRect(0, g -> get<2> ()[0] - (g -> get<5> ()), tmp.width(), g -> get<
			5> () * 2);
	qp.setOpacity(1);

	qp.setPen("red");
	qp.drawLine(0, g -> get<2> ()[0], tmp.width(), g -> get<2> ()[0]);

	qp.setPen(Qt::DashLine);
	qp.drawLine(0, g -> get<2> ()[1], tmp.width(), g -> get<2> ()[1]);
	qp.drawLine(0, g -> get<2> ()[2], tmp.width(), g -> get<2> ()[2]);

	qp.setPen(QPen(QColor("blue"), 1.5, Qt::SolidLine));
	qp.drawLine(g -> get<3> ()[0], 0, g -> get<3> ()[0], tmp.height());
	qp.drawLine(g -> get<3> ()[2], 0, g -> get<3> ()[2], tmp.height());

	qp.setPen(QPen(QColor("blue"), 1.5, Qt::DotLine));
	qp.drawLine(g -> get<3> ()[1], 0, g -> get<3> ()[1], tmp.height());
	qp.drawLine(g -> get<3> ()[3], 0, g -> get<3> ()[3], tmp.height());

	imgl -> setPixmap(QPixmap::fromImage(tmp.scaled(imgl -> size(),
			Qt::KeepAspectRatio, Qt::SmoothTransformation)));

	//--------

	xgdtl -> resize(150, imgl -> pixmap() -> height());
	xgdtl -> move(imgl -> pixmap() -> width(), 0);

	ygdtl -> resize(imgl -> pixmap() -> width(), 150);
	ygdtl -> move(0, imgl -> pixmap() -> height());

	xgdtl -> setPixmap(xgdtli -> scaled(xgdtl -> size(), Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation));
	ygdtl -> setPixmap(ygdtli -> scaled(ygdtl -> size(), Qt::IgnoreAspectRatio,
			Qt::SmoothTransformation));

	wgt -> exec();

}
