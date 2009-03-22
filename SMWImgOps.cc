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

QRgb SobMainWin::To_gray(QRgb r) {
	int v = QColor::fromRgb(r).toHsv().value();
	return QColor(v, v, v).rgb();
}

void SobMainWin::To_gray(bool d) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	for (int y = 0; y < in_im -> height(); ++y) {

		for (int x = 0; x < in_im -> width(); ++x) {
			//int v = QColor::fromRgb(in_im -> pixel(x,y)).toHsv().value();
			int v = qGray(in_im -> pixel(x, y));
			in_im -> setPixel(x, y, QColor(v, v, v).rgb());
		}
	}

	out_im.reset(new QImage(*in_im));
	Pss(tmr.elapsed());

	if (!d)
		Display_imgs();

	this -> setCursor(Qt::ArrowCursor);

}

/*void SobMainWin::Gamma( float f )
 {

 QTime tmr;
 tmr.start();

 for(int y = 0; y < in_im -> height(); ++y) //rows
 {
 for(int x = 0; x < in_im -> width(); ++x) //cols
 {
 QRgb val = in_im -> pixel(x, y);
 QColor nval;

 nval.setRed(n_rgb(255 * std::pow(qRed(val) / 255.0, f)));
 nval.setBlue(n_rgb(255 * std::pow(qBlue(val) / 255.0, f)));
 nval.setGreen(n_rgb(255 * std::pow(qGreen(val) / 255.0, f)));

 in_im -> setPixel(x, y, nval.rgb());
 }
 }

 Pss(tmr.elapsed());
 Display_imgs();

 }*/

void SobMainWin::Smooth() {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	int8_t Smth[3][3] = { { 1, 1, 1 }, { 1, -5, 1 }, { 1, 1, 1 } };
	uint8_t px = 0;

	for (int y = 0; y < in_im -> height(); y++) {
		for (int x = 0; x < in_im -> width(); x++) {
			if ((y == 0) or (y + 1 >= in_im -> height())) {
				px = 0;
			} else if ((x == 0) or (x + 1 >= in_im -> width())) {
				px = 0;
			} else {
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						px = (qRed(in_im -> pixel(x + i, y + j))
								* Smth[i + 1][j + 1]);
					}
				}
			}

			out_im -> setPixel(x, y, QColor(px, px, px).rgb());
		}
	}

	Pss(tmr.elapsed());
	Display_imgs();
	this -> setCursor(Qt::ArrowCursor);
}

void SobMainWin::Otsus_bin(bool d) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	//To_gray();

	const uint16_t GLVL = 256;
	const uint64_t IMSIZE = out_im -> height() * out_im -> width();

	uint thd = 0;

	uint64_t hist[256];
	std::fill(hist, hist + GLVL, 0);

	double prob[GLVL];
	std::fill(prob, prob + GLVL, 0.0);

	double mean[GLVL];
	std::fill(mean, mean + GLVL, 0.0);

	double omega[GLVL];
	std::fill(omega, omega + GLVL, 0.0);

	double max_sigma = 0;
	double sigma[GLVL];
	std::fill(sigma, sigma + GLVL, 0.0);

	//histogram
	for (int y = 0; y < out_im -> height(); ++y) {
		for (int x = 0; x < out_im -> width(); ++x) {
			hist[qRed(out_im -> pixel(x, y))]++;
		}
	}
	/*
	 for(uint16_t i = 0; i < GLVL; ++i)
	 {
	 std::cout << i << ": " << hist[i]<<std::endl;
	 }*/

	//prawdopodobieństwo
	for (uint16_t i = 0; i < GLVL; ++i) {
		prob[i] = static_cast<double> (hist[i]) / IMSIZE;
	}

	//srednie
	mean[0] = 0;
	omega[0] = prob[0];
	for (uint16_t i = 1; i < GLVL; ++i) {
		mean[i] = mean[i - 1] + i * prob[i];
		omega[i] = omega[i - 1] + prob[i];
	}

	//maksymalizacja sigma
	for (uint16_t i = 0; i < GLVL - 1; ++i) {
		if ((omega[i] != 0.0) and (omega[i] != 1.0)) {
			sigma[i] = std::pow(mean[GLVL - 1] * omega[i] - mean[i], 2)
					/ (omega[i] * (1.0 - omega[i]));
		} else {
			sigma[i] = 0.0;
		}

		if (sigma[i] > max_sigma) {
			thd = i;
			max_sigma = sigma[i];
		}
	}

	for (int y = 0; y < out_im -> height(); ++y) {
		for (int x = 0; x < out_im -> width(); ++x) {
			if ((unsigned) qRed(out_im -> pixel(x, y)) > thd) {
				out_im -> setPixel(x, y, QColor(255, 255, 255).rgb());
			} else {
				out_im -> setPixel(x, y, QColor(0, 0, 0).rgb());
			}
		}
	}

	(this -> out_im).reset(new QImage(*out_im));

	Pss(tmr.elapsed(), "[thd:" + QString::number(thd) + "]");
	if (!d)
		Display_imgs();
	//statusBar() -> showMessage("[thd:" + QString::number(thd) + "]", 5000);
	bin = true;
	this -> setCursor(Qt::ArrowCursor);

}

void SobMainWin::Lame_bin(bool d) {
	std::cerr << "FAIL" << std::endl;
	QTime tmr;
	tmr.start();

	//To_gray();

	const uint16_t GLVL = 256;
	bool first = false;

	int64_t max = 0;

	uchar thd = 0;

	int64_t hist[256];
	std::fill(hist, hist + GLVL, 0);

	//histogram
	for (int y = 0; y < out_im -> height(); ++y) {
		for (int x = 0; x < out_im -> width(); ++x) {
			hist[qRed(out_im -> pixel(x, y))]++;
		}
	}

	for (int i = 0; i < GLVL - 1; i++) {

		if (hist[i] > max) {
			if (hist[i] > hist[i + 1]) // czy jeszcze będzie rosnąć
			{
				if (first) {
					thd = i;
					break;
				} else {
					max = hist[i];
					first = true;
				}
			}
		}
	}

	for (int y = 0; y < out_im -> height(); ++y) {
		for (int x = 0; x < out_im -> width(); ++x) {
			if ((unsigned) qRed(out_im -> pixel(x, y)) > thd) {
				out_im -> setPixel(x, y, QColor(255, 255, 255).rgb());
			} else {
				out_im -> setPixel(x, y, QColor(0, 0, 0).rgb());
			}
		}
	}

	(this -> out_im).reset(new QImage(*out_im));

	Pss(tmr.elapsed());
	if (!d)
		Display_imgs();
	statusBar() -> showMessage("[thd:" + QString::number(thd) + "]", 5000);
	bin = true;

}

void SobMainWin::Sobel_op(bool d) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	//if(not bin) To_gray();
	if (mwin_ui -> checkBox -> isEnabled())
		Smooth();

	vgrads_t grads = Make_grads(false)-> get<3> ();
	boost::scoped_ptr<QImage> tmp(new QImage(*out_im));

	int sumx = 0, sumy = 0, sum = 0;

	for (int y = 1; y < in_im -> height() - 1; y++) {
		for (int x = 1; x < in_im -> width() - 1; x++) {
			sumx = sumy = 0;

			if ((y == 0) or (y + 1 >= in_im -> height())) {
				sum = 0;
			} else if ((x == 0) or (x + 1 >= in_im -> width())) {
				sum = 0;
			} else {

				sumx = grads.first[x][y];
				sumy = grads.second[x][y];

				//	sum = std::abs(sumx) + std::abs(sumy);
				sum = static_cast<int> (255 - std::sqrt(std::pow(sumx, 2.0)
						+ std::pow(sumy, 2.0)));
				sum = sum > 255 ? 255 : sum;
				sum = sum < 0 ? 0 : sum;
			}

			sum = static_cast<uchar> (sum);

			out_im -> setPixel(x, y, qRgb(sum, sum, sum));

		}
	}

	Pss(tmr.elapsed());
	if (!d)
		Display_imgs();
	sobel = true;
	this -> setCursor(Qt::ArrowCursor);
}

void SobMainWin::Avg_blur(bool d) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	uint sum = 0;
	const double pct = (9 * 255) * 0.85;
	uchar curr = 0;

	QImage tmpim(*out_im);

	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			if ((y == 0) or (y + 1 >= out_im -> height())) {
				sum = 255;
			} else if ((x == 0) or (x + 1 >= out_im -> width())) {
				sum = 255;
			} else {
				sum = 0;
				curr = qRed(out_im -> pixel(x, y));

				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						sum += qRed(out_im -> pixel(x + i, y + j));
					}
				}
			}

			if ((sum) > pct) {
				tmpim.setPixel(x, y, QColor(255, 255, 255).rgb());
			} else {
				tmpim.setPixel(x, y, QColor(curr, curr, curr).rgb());
			}
		}
	}

	out_im.reset(new QImage(tmpim));
	Pss(tmr.elapsed());
	if (!d)
		Display_imgs();

	this -> setCursor(Qt::ArrowCursor);

}

void SobMainWin::Gauss_blur(bool d) {
	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	//	const int KERNS = 5;
	//	const int KERNMID = std::ceil(KERNS / 2);
	//	const double SIGMA = 0.4;
	//
	//	double **gknl = new double *[KERNS];
	//	std::fill(gknl, gknl + KERNS, new double[KERNS]);
	//
	//	double sum = 0;
	//	for(int i = 0; i < KERNS; ++i)
	//	{
	//		for(int j = 0; j < KERNS; ++j)
	//		{
	//			int x = i - KERNMID;
	//			int y = j - KERNMID;
	//
	//			double m = std::sqrt(2.0 * M_PI * std::pow(SIGMA, 2));
	//			double a = 1.0 / m;
	//			double p = -((std::pow(x, 2) + std::pow(y, 2)) / (2.0 * std::pow(
	//					SIGMA, 2)));
	//
	//			gknl[i][j] = a * std::exp(p);
	//			sum += a * std::exp(p);
	//		}
	//	}

	uint gknl[][5] = { { 2, 4, 5, 4, 2 }, { 4, 9, 12, 9, 4 }, { 5, 12, 15, 12,
			5 }, { 2, 4, 5, 4, 2 }, { 4, 9, 12, 9, 4 } };
	const uint sum = 159;

	int px = 0;
	QImage tmp(*out_im);
	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			px = 0;
			if ((y < 2) or (y + 2 >= out_im -> height())) {
				px = qRed(out_im -> pixel(x, y));
			} else if ((x < 2) or (x + 2 >= out_im -> width())) {
				px = qRed(out_im -> pixel(x, y));
			} else {
				for (int i = -2; i <= 2; i++) {
					for (int j = -2; j <= 2; j++) {
						px += (qRed(out_im -> pixel(x + i, y + j))
								* gknl[i + 2][j + 2]);
					}
				}
				px /= sum;
			}

			tmp.setPixel(x, y, QColor(px, px, px).rgb());
		}
	}

	//delete[] gknl;

	out_im.reset(new QImage(tmp));
	if (!d)
		Display_imgs();
	Pss(tmr.elapsed());
	this -> setCursor(Qt::ArrowCursor);
}

void SobMainWin::Median_fr(bool d) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	uint sum = 0;

	QImage tmpim(*out_im);

	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			if ((y == 0) or (y + 1 >= out_im -> height())) {
				sum = 255;
			} else if ((x == 0) or (x + 1 >= out_im -> width())) {
				sum = 255;
			} else {
				sum = 0;
				boost::scoped_array<uchar> vct(new uchar[9]);
				std::fill(vct.get(), vct.get() + 9, 0);
				uchar ctr = 0;

				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						vct[ctr] = qRed(out_im -> pixel(x + i, y + j));
						ctr++;
					}
				}

				std::sort(vct.get(), vct.get() + 9);

				sum = vct[4];

			}

			tmpim.setPixel(x, y, QColor(sum, sum, sum).rgb());
		}
	}

	out_im.reset(new QImage(tmpim));
	Pss(tmr.elapsed());
	if (!d)
		Display_imgs();
	this -> setCursor(Qt::ArrowCursor);
}

boost::shared_ptr<SobMainWin::hought_t> SobMainWin::Hough_tm(bool d, uint rad) {

	this -> setCursor(Qt::WaitCursor);

	const uint IMSIZE = out_im -> height() * out_im -> width();
	typedef hought_t res_t;
	typedef std::vector<std::vector<uint> > acc_t;

	//boost::scoped_array<uint> acc(new uint[IMSIZE]);
	//std::fill( acc.get(), acc.get() +IMSIZE, 0);

	acc_t acc(out_im -> width(), std::vector<uint>(out_im -> height(), 0));
	boost::shared_ptr<res_t> res(new res_t(20, boost::make_tuple(0, 0, 0))); // x, y, val

	double x0, y0;
	double rth;

	uint max = 0;
	const uchar MAXELS = 20;

	std::cout << IMSIZE * sizeof(uint) / 1024.0 / 1024.0 << "MB (" << IMSIZE
			<< " * " << sizeof(uint) << ")" << std::endl;

	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			if (qRed(out_im -> pixel(x, y)) == 0) {
				for (uint16_t theta = 0; theta < 360; ++theta) {
					rth = (theta * M_PI) / 180.0;
					x0 = rint(x - rad * std::cos(rth));
					y0 = rint(y - rad * std::sin(rth));

					if ((x0 > 0) and (x0 < out_im -> width()) and (y0 > 0)
							and (y0 < out_im -> height())) {
						acc[static_cast<size_t>(x0)][static_cast<size_t>(y0)]++;
						//std::cout << x0 << " " << y0 << "; " << x0  + y0 * out_im -> width()<< std::endl;
					}
				}
			}
		}
	}

	//max
	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			max = acc[x][y] > max ? acc[x][y] : max;
		}
	}

	std::cout << max << std::endl;

	//norm
	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			acc[x][y] = static_cast<acc_t::value_type::value_type> ((acc[x][y]
					/ (max * 1.0)) * 255.0);
		}
	}

	//vals
	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			if (acc[x][y] > res -> front().get<2> ()) {
				res_t::iterator it;
				bool broke = false;

				for (it = res -> begin(); it != res-> end(); it++) {
					if (acc[x][y] < (it -> get<2> ())) {
						broke = true;
						break;
					}
				}

				if (broke) {
					res -> insert(it, boost::make_tuple(x, y, acc[x][y]));
				} else if (it == res->end()) {
					res -> push_back(boost::make_tuple(x, y, acc[x][y]));
				}

				while (res->size() > MAXELS) {
					res->pop_front();
				}

			}
		}
	}

	QImage tmpi(*out_im);
	if (!d) {
		QPainter qp;
		qp.begin(&tmpi);

		qp.setPen(QColor("red"));

		res_t::iterator it = res->begin();
		for (int i = 0; i < MAXELS; i++) {
			std::cout << i << ": " << (it -> get<0> ()) << " "
					<< (it -> get<1> ()) << ": " << (it -> get<2> ())
					<< std::endl;
			if (it -> get<2> () > 250) {
				qp.setPen(QColor("red"));
			} else {
				qp.setPen(QColor("red"));
			}

			qp.drawEllipse(it -> get<0> (), it -> get<1> (), rad, rad);
			it++;
		}

		qp.end();
	}

	out_im.reset(new QImage(tmpi));
	if (!d)
		Display_imgs();

	this -> setCursor(Qt::ArrowCursor);
	return res;
}

boost::shared_ptr<SobMainWin::grad_t> SobMainWin::Make_grads(bool) {

	QTime tmr;
	tmr.start();
	this -> setCursor(Qt::WaitCursor);

	gradarr_t gt_x(new gradarr_t::element_type[out_im -> height()]);
	gradarr_t gt_y(new gradarr_t::element_type[out_im -> width()]);

	igrads_t igt;

	vgrad_t vgradx(out_im->width(), vgrad_t::value_type(out_im->height()));
	vgrad_t vgrady(out_im->width(), vgrad_t::value_type(out_im->height()));

	boost::shared_ptr<grad_t> rp(new grad_t(gt_x, gt_y, igt, std::make_pair(vgradx, vgrady)));

	std::fill(gt_x.get(), gt_x.get() + out_im -> height(), 0);
	std::fill(gt_y.get(), gt_y.get() + out_im -> width(), 0);

	int8_t Gx[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
	int8_t Gy[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };

	int sumx, sumy = 0;
	QImage xgrad(*out_im);
	QImage ygrad(*out_im);

	for (int y = 0; y < out_im -> height(); y++) {
		for (int x = 0; x < out_im -> width(); x++) {
			sumx = sumy = 0;

			if ((y == 0) or (y + 1 >= out_im -> height())) {
				//sum = 0;
			} else if ((x == 0) or (x + 1 >= out_im -> width())) {
				//sum = 0;
			} else {
				for (int i = -1; i <= 1; i++) {
					for (int j = -1; j <= 1; j++) {
						QRgb mspx = out_im -> pixel(x + i, y + j);

						sumx += qRed(mspx) * Gx[i + 1][j + 1];
						sumy += qRed(mspx) * Gy[i + 1][j + 1];
					}
				}
			}

			rp -> get<3>().first[x][y] = sumx;
			rp -> get<3>().second[x][y] = sumy;

			if (sumx > 255)
				sumx = 255;
			if (sumx < 0)
				sumx = 0;

			if (sumy > 255)
				sumy = 255;
			if (sumy < 0)
				sumy = 0;

			xgrad.setPixel(x, y, QColor(sumx, sumx, sumx).rgb());
			ygrad.setPixel(x, y, QColor(sumy, sumy, sumy).rgb());
		}
	}



	for (int y = 0; y < out_im -> height(); y++) {
		gt_x[y] = 0;
		for (int x = 0; x < out_im -> width(); x++) {
			gt_x[y] += qRed(xgrad.pixel(x, y));
		}

	}

	const uint mx = Find_eyeline_el(out_im -> height() / 4, out_im -> height()
			/ 4 * 2, gt_x);
	const uint pct = static_cast<int> (((out_im -> height())
			* (XTOLPCT / 100.0)));
	const int up = mx + pct;
	const int down = mx - pct;
	for (int y = 0; y < out_im -> width(); y++) {
		gt_y[y] = 0;
		for (int x = 0; x < out_im -> height(); x++) {
			if ((x > down) and (x < up)) {
				gt_y[y] += static_cast<ulong> (qRed(ygrad.pixel(y, x)) * 2);
			} else {
				gt_y[y] += static_cast<ulong> (qRed(ygrad.pixel(y, x)) * 0.5);
			}
		}
	}

	rp -> get<2> ().first.reset(new QImage(xgrad));
	rp -> get<2> ().second.reset(new QImage(ygrad));

	Pss(tmr.elapsed());
	this -> setCursor(Qt::ArrowCursor);

	return rp;

}

void SobMainWin::Canny_ed(bool d) {
	this -> setCursor(Qt::WaitCursor);
	QProgressDialog qpd(this, Qt::Dialog);

	std::vector<std::vector<uint8_t> > edir(out_im->width(), std::vector<
			uint8_t>(out_im->height(), 0)); //edge direction
	std::vector<std::vector<uint64_t> > grad(out_im->width(), std::vector<
			uint64_t>(out_im->height(), 0)); //gradient value

	To_gray(false);
	Gauss_blur(false);
	//	Otsus_bin(false);

	boost::shared_ptr<SobMainWin::grad_t> xygrads = Make_grads(false);

	QImage tmpi(*out_im);

	const uint thd1 = QInputDialog::getInt(this, "threshold", "thd1", 80, 0,
			255);
	const uint thd2 = QInputDialog::getInt(this, "threshold", "thd2", 30, 0,
			255);

	int8_t SGx[3][3] = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
	int8_t SGy[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };

	qpd.setMaximum((tmpi.height() * tmpi.width()) * 4);
	ulong ctr = 0;
	qpd.setCancelButton(0);
	qpd.show();

	qpd.setLabelText(QString::fromUtf8("Wyliczanie kąta krawędzi"));
	for (size_t x = 1; x < edir.size() - 1; ++x) {
		for (size_t y = 1; y < edir[x].size() - 1; ++y) {

			qpd.setValue(ctr++);

			int gx = 0, gy = 0;
			for (char sox = -1; sox <= 1; sox++) {
				for (char soy = -1; soy <= 1; soy++) {
					QRgb mspx = tmpi.pixel(x + sox, y + soy);
					gx += qRed(mspx) * SGx[sox + 1][soy + 1];
					gy += qRed(mspx) * SGy[sox + 1][soy + 1];
				}
			}

			//		const ulong gx = qRed(xygrads -> get<2> ().first->pixel(x, y));
			//	const ulong gy = qRed(xygrads -> get<2> ().second->pixel(x, y));

			grad[x][y] = static_cast<ulong>(std::sqrt(std::pow((double) gx, 2.0) + std::pow(gy,
					2.0)));

			const double thisAngle = (std::atan2(gx, gy) / M_PI) * 180.0;
			uint8_t newAngle = 0;

			if (((thisAngle < 22.5) && (thisAngle > -22.5)) || (thisAngle
					> 157.5) || (thisAngle < -157.5))
				newAngle = 0;
			if (((thisAngle > 22.5) && (thisAngle < 67.5)) || ((thisAngle
					< -112.5) && (thisAngle > -157.5)))
				newAngle = 45;
			if (((thisAngle > 67.5) && (thisAngle < 112.5)) || ((thisAngle
					< -67.5) && (thisAngle > -112.5)))
				newAngle = 90;
			if (((thisAngle > 112.5) && (thisAngle < 157.5)) || ((thisAngle
					< -22.5) && (thisAngle > -67.5)))
				newAngle = 135;

			edir[x][y] = newAngle;

		}
	}

	qpd.setLabelText(QString::fromUtf8("Śledzenie krawędzi"));
	for (size_t x = 0; x < edir.size(); ++x) {
		for (size_t y = 0; y < edir[x].size(); ++y) {
			qpd.setValue(ctr++);

			if (x > 0 && y > 0 && (grad[x][y] > thd1)) {
				switch (edir[x][y]) {
				case 0:
					canny_edge_trace(tmpi, 0, x, y, std::make_pair(0, 1), edir,
							grad, thd1, thd2);
					break;
				case 45:
					canny_edge_trace(tmpi, 45, x, y, std::make_pair(1, 1),
							edir, grad, thd1, thd2);
					break;
				case 90:
					canny_edge_trace(tmpi, 90, x, y, std::make_pair(1, 0),
							edir, grad, thd1, thd2);
					break;
				case 135:
					canny_edge_trace(tmpi, 135, x, y, std::make_pair(1, -1),
							edir, grad, thd1, thd2);
					break;
				default:
					tmpi. setPixel(x, y, qRgb(255, 255, 255));
					break;
				}
			} else {
				tmpi. setPixel(x, y, qRgb(255, 255, 255));
			}
		}
	}

	qpd.setLabelText(QString::fromUtf8("Usuwanie niezmienionych"));
	for (size_t x = 0; x < edir.size(); ++x) {
		for (size_t y = 0; y < edir[x].size(); ++y) {
			qpd.setValue(ctr++);

			QRgb qr = tmpi.pixel(x, y);

			if (qRed(qr) != 0 && qRed(qr) != 255) {
				tmpi.setPixel(x, y, qRgb(255, 255, 255));
			}

		}
	}

	qpd.setLabelText(QString::fromUtf8("Supresja niemaksymalnych"));
	for (size_t x = 0; x < edir.size(); ++x) {
		for (size_t y = 0; y < edir[x].size(); ++y) {
			qpd.setValue(ctr++);
			if (qRed(tmpi.pixel(x, y)) == 255) {
				switch (edir[x][y]) {
				case 0:
					canny_supr_nonmax(tmpi, 0, x, y, std::make_pair(1, 0),
							edir, grad, thd2);
					break;
				case 45:
					canny_supr_nonmax(tmpi, 45, x, y, std::make_pair(1, -1),
							edir, grad, thd2);
					break;
				case 90:
					canny_supr_nonmax(tmpi, 90, x, y, std::make_pair(0, 1),
							edir, grad, thd2);
					break;
				case 135:
					canny_supr_nonmax(tmpi, 135, x, y, std::make_pair(1, 1),
							edir, grad, thd2);
					break;
				default:
					break;
				}

			}
		}
	}

	out_im.reset(new QImage(tmpi));
	if (!d)
		Display_imgs();

	this -> setCursor(Qt::ArrowCursor);
}

void SobMainWin::canny_edge_trace(QImage &qi, uint8_t dir, uint64_t row,
		uint64_t col, std::pair<int8_t, int8_t> shift, const std::vector<
				std::vector<uint8_t> > &edirs, const std::vector<std::vector<
				uint64_t> > & grads, const uint64_t, const uint64_t t2) {
	uint64_t lrow, lcol;
	lrow = lcol = 0;
	bool doit = true;

	lrow = canny_et_mkrowcol(qi.width(), row, shift.first, doit);
	lcol = canny_et_mkrowcol(qi.height(), col, shift.second, doit);

	doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
			== std::numeric_limits<uint64_t>::max()));

	while (doit && (edirs[lrow][lcol] == dir) && (grads[lrow][lcol] > t2)) {
		qi.setPixel(lrow, lcol, qRgb(0, 0, 0));

		lrow = canny_et_mkrowcol(qi.width(), lrow, shift.first, doit);
		lcol = canny_et_mkrowcol(qi.height(), lcol, shift.second, doit);

		doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
				== std::numeric_limits<uint64_t>::max()));

	}

}

void SobMainWin::canny_supr_nonmax(QImage &qi, uint8_t dir, uint64_t row,
		uint64_t col, std::pair<int8_t, int8_t> shift, const std::vector<
				std::vector<uint8_t> > &edirs, const std::vector<std::vector<
				uint64_t> > & grads, const uint64_t) {

	uint64_t lrow, lcol;
	lrow = lcol = 0;
	bool doit = true;
	std::vector<std::vector<float> > nmax(qi.width(),
			std::vector<float>(3, 0.0));
	std::vector<int> max(3, 0);
	uint64_t nmaxctr = 0;

	lrow = canny_et_mkrowcol(qi.width(), row, shift.first, doit);
	lcol = canny_et_mkrowcol(qi.height(), col, shift.second, doit);

	doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
			== std::numeric_limits<uint64_t>::max()));

	while (doit && (edirs[lrow][lcol] == dir) && (qRed(qi.pixel(lrow, lcol))
			== 0)) {
		lrow = canny_et_mkrowcol(qi.width(), lrow, shift.first, doit);
		lcol = canny_et_mkrowcol(qi.height(), lcol, shift.second, doit);

		doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
				== std::numeric_limits<uint64_t>::max()));

		nmax[nmaxctr][0] = lrow;
		nmax[nmaxctr][1] = lcol;
		nmax[nmaxctr][2] = grads[lrow][lcol];
		nmaxctr++;
	}

	doit = true;
	shift = std::make_pair(-1, -1);
	lrow = canny_et_mkrowcol(qi.width(), row, shift.first, doit);
	lcol = canny_et_mkrowcol(qi.height(), col, shift.second, doit);

	doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
			== std::numeric_limits<uint64_t>::max()));
	while (doit && (edirs[lrow][lcol] == dir) && (qRed(qi.pixel(lrow, lcol))
			== 0)) {
		lrow = canny_et_mkrowcol(qi.width(), lrow, shift.first, doit);
		lcol = canny_et_mkrowcol(qi.height(), lcol, shift.second, doit);

		doit = (not (lrow == std::numeric_limits<uint64_t>::max() || lcol
				== std::numeric_limits<uint64_t>::max()));

		nmax[nmaxctr][0] = lrow;
		nmax[nmaxctr][1] = lcol;
		nmax[nmaxctr][2] = grads[lrow][lcol];
		nmaxctr++;
	}

	for (uint64_t ctr = 0; ctr < nmaxctr; ++ctr) {
		if (nmax[ctr][2] > max[2]) {
			std::copy(nmax[ctr].begin(), nmax[ctr].end(), max.begin());
		}
	}

	for (uint64_t ctr = 0; ctr < nmaxctr; ++ctr) {
		qi.setPixel(static_cast<int>(nmax[ctr][0]), static_cast<int>(nmax[ctr][1]), qRgb(255, 255, 255));
	}

}

uint64_t SobMainWin::canny_et_mkrowcol(uint64_t max, uint64_t curr,
		int8_t shift, bool & eend) const {

	if (shift < 0) {
		if (curr > 0) {
			return curr + shift;
		} else {
			eend = true;
			return std::numeric_limits<uint64_t>::max();
		}
	} else if (curr < (max - 1)) {
		return curr + shift;
	} else {
		eend = true;
		return std::numeric_limits<uint64_t>::max();
	}

}
