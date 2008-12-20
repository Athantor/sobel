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

boost::shared_ptr<SobMainWin::feat_t> SobMainWin::Make_feats( bool )
{
	boost::shared_ptr<SobMainWin::grad_t> g = Make_grads(true);
	gradarr_t gt_x = g -> get<0> ();
	gradarr_t gt_y = g -> get<1> ();
	featarr_t mx(new featarr_t::element_type[6]);
	featarr_t my(new featarr_t::element_type[4]);

	boost::shared_ptr<feat_t> rp(new feat_t(mx, my, 0, 0, 0));

	std::fill(mx.get(), mx.get() + 6, 0);
	std::fill(my.get(), my.get() + 4, 0);

	const uchar TOLPCT = 5; //tolerance above and below where eyes shall be
	rp -> get<2> () = static_cast<int> (((out_im -> height())
			* (TOLPCT / 100.0))); //eye area height

	const int YWDT = static_cast<int> (((out_im -> width()) * (7 / 100.0))); // Y tolerance

	// eye line: looking in space between 1/4 and 3/4 image's height
	mx[0] = Find_eyeline_el(out_im -> height() / 4, out_im -> height() / 4 * 2, gt_x);

	my[1] = std::max_element(gt_y.get() + 5, gt_y.get() + (out_im -> width()
			/ 3) + 1) - gt_y.get(); // outermost left max
	my[3] = std::max_element((gt_y.get() + (out_im -> width() - 5))
			- (out_im -> width() / 3) - 1, (gt_y.get()
			+ (out_im -> width() - 5))) - gt_y.get(); // outermost right max

	const int centr = my[1] + (my[3] - my[1]) / 2; //face center

	my[0] = std::max_element(gt_y.get() + my[1] + YWDT, gt_y.get() + centr
			- YWDT / 2) - gt_y.get(); // inner left max
	my[2] = std::max_element(gt_y.get() + centr + YWDT / 2, gt_y.get() + my[3]
			- YWDT) - gt_y.get(); //inner right max

	//---------------- x

	mx[1] = std::max_element(gt_x.get() + (mx[0] - (rp -> get<2> ()) * 2),
			gt_x.get() + mx[0] - static_cast<int> ((rp -> get<2> ()) * 0.8))
			- gt_x.get(); // eye brows

	mx[2] = std::max_element(gt_x.get() + 3, gt_x.get() + mx[1] - 1)
			- gt_x.get(); // hair

	mx[3] = std::max_element(gt_x.get() + mx[0] + static_cast<int> ((my[3]
			- my[0]) / 2.0) - (rp -> get<2> ()), gt_x.get() + mx[0]
			+ static_cast<int> ((my[3] - my[0]) / 2.0)) - gt_x.get(); // nose

	mx[4] = std::max_element(gt_x.get() + mx[3] + 1 + static_cast<int> ((my[3]
			- my[0]) / 5.0), gt_x.get() + mx[3] + 1 + static_cast<int> ((my[3]
			- my[0]) / 3.0)) - gt_x.get(); //mouth

	mx[5] = std::max_element(gt_x.get() + mx[4] + static_cast<int> (((mx[3]
			- mx[0]) * 0.8)), gt_x.get() + mx[4] + static_cast<int> (((mx[3]
			- mx[0]) * 1.2))) - gt_x.get(); //chin

	rp -> get<3>() = centr;
	rp -> get<4>() = mx[2] + ( (mx[5] - mx[2]) / 2  );

	return rp;

}
