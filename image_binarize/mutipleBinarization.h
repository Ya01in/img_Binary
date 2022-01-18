#pragma once
#include "packageEtFunctions.h"
#include <limits>
using namespace std;
/*********************************************** Yen *****************************************************/

Mat Yen(Mat& gray)
{
	cv::Mat output;

	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	for (num = 0; num < 256; num++)
		hist_use[num] = hist[num];
	double* hist_norm = (double*)malloc(256 * sizeof(double));
	double* hist_cal = (double*)malloc(256 * sizeof(double));
	double* hist_sq = (double*)malloc(256 * sizeof(double));
	double* hist_sq_2 = (double*)malloc(256 * sizeof(double));

	long tol = gray.rows * gray.cols;

	for (num = 0; num < 256; num++)
	{
		//cout << "all num = " << hist_use[num] << endl;
		hist_norm[num] = double(hist_use[num]) / tol;
		hist_cal[num] = (!num) ? hist_norm[num] : hist_cal[num - 1] + hist_norm[num]; //num > 0 會累加前一項
		hist_sq[num] = (!num) ? pow(hist_norm[num], 2) : hist_sq[num - 1] + pow(hist_norm[num], 2); //num > 0 會累加前一項 -->平方
		//cout << "all num = " << hist_norm[num] << " " << hist[num] << " " << tol << endl;
	}

	hist_sq_2[256 - 1] = 0.0;
	for (int i = 256 - 2; i >= 0; i--) hist_sq_2[i] = hist_sq_2[i + 1] + pow(hist_norm[i + 1], 2);

	int threshold = 0;
	double max_crit = 0; //initial min value
	double crit = 0;

	for (num = 0; num < 256; num++)
	{

		crit = -1.0 * ((hist_sq[num] * hist_sq_2[num]) > 0.0 ? log(hist_sq[num] * hist_sq_2[num]) : 0.0) + 2 * ((hist_cal[num] * (1.0 - hist_cal[num])) > 0.0 ? log(hist_cal[num] * (1.0 - hist_cal[num])) : 0.0);
		//cout << "crit value = " << crit << endl;
		if (crit > max_crit)
		{
			max_crit = crit;
			threshold = num;
		}
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);


	return output;

}

/*********************************************** shanbhag *****************************************************/
Mat shanbhag(Mat& gray)
{
	cv::Mat output;
	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	for (num = 0; num < 256; num++) hist_use[num] = hist[num];
	double* P1 = (double*)malloc(256 * sizeof(double));
	double* P2 = (double*)malloc(256 * sizeof(double));
	double* hist_norm = (double*)malloc(256 * sizeof(double));
	long tol = gray.rows * gray.cols;
	int first_bin = 0;
	int stop = 0;
	for (num = 0; num < 256; num++)
	{
		hist_norm[num] = double(hist_use[num]) / tol;
		P1[num] = (!num) ? hist_norm[0] : (P1[num - 1] + hist_norm[num]); //num > 0 �|�֥[�e�@��
		P2[num] = (!num) ? (1.0 - P1[0]) : (1.0 - P1[num]); //num > 0 �|�֥[�e�@�� -->����
		/* Determine the first non-zero bin */
		if (!(abs(P1[num]) < 2.220446049250313E-16) && (stop == 0))
		{
			first_bin = num;
			stop++;
		}
	}
	/* Determine the last non-zero bin */
	int last_bin = 255;
	for (num = 255; num >= first_bin; num--) {
		if (!(abs(P2[num]) < 2.220446049250313E-16)) {
			last_bin = num;
			break;
		}
	}

	//cout << first_bin << " " << last_bin << endl;
	// Calculate the total entropy each gray-level and find the threshold that maximizes it 
	int threshold = -1;
	double min_ent = 255;
	double ent_back, term, ent_obj, tot_ent;
	for (num = first_bin; num <= last_bin; num++)
	{
		ent_back = 0.0;
		term = 0.5 / P1[num];
		for (int i = 1; i <= num; i++) ent_back -= hist_norm[i] * log(1.0 - term * P1[i - 1]);

		ent_back *= term;

		/* Entropy of the object pixels */
		ent_obj = 0.0;
		term = 0.5 / P2[num];
		for (int i = num + 1; i < 256; i++)  ent_obj -= hist_norm[i] * log(1.0 - term * P2[i]);

		ent_obj *= term;

		/* Total entropy */
		tot_ent = abs(ent_back - ent_obj);
		//cout << tot_ent << "\n" << endl;
		if (tot_ent < min_ent)
		{
			min_ent = tot_ent;
			threshold = num;
		}
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);


	return output;
}

/*********************************************** percentile *****************************************************/

int partialSum(int* img, int stop)
{
	int x = 0;
	for (int i = 0; i <= stop; i++)
		x += img[i];
	return x;
}

Mat percentile(Mat& gray)
{
	cv::Mat output;

	int iter = 0;
	int threshold = -1;
	double ptile = 0.5; // default fraction of foreground pixels
	double* avec = (double*)malloc(256 * sizeof(double));
	double x = 0.0;
	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	double temp = 1.0;

	for (num = 0; num < 256; num++) hist_use[num] = hist[num];

	double total = partialSum(hist_use, 255);

	for (num = 0; num < 256; num++)
	{
		avec[num] = abs((partialSum(hist_use, num) / total) - ptile);
		//IJ.log("Ptile["+i+"]:"+ avec[i]);
		if (avec[num] < temp)
		{
			temp = avec[num];
			threshold = num;
		}
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);

	return output;
}

/*********************************************** moments *****************************************************/
Mat moments(Mat& gray)
{
	cv::Mat output;

	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	for (num = 0; num < 256; num++)
		hist_use[num] = hist[num];
	double* histo = (double*)malloc(256 * sizeof(double));
	double total = 0;
	double m0 = 1.0, m1 = 0.0, m2 = 0.0, m3 = 0.0, sum = 0.0, p0 = 0.0;
	double cd, c0, c1, z0, z1;	/* auxiliary variables */
	int threshold = -1;

	long tol = gray.rows * gray.cols;

	for (num = 0; num < 256; num++)
	{
		//cout << "all num = " << hist_use[num] << endl;
		histo[num] = double(hist_use[num]) / tol;
		m1 += num * histo[num];
		m2 += num * num * histo[num];
		m3 += num * num * num * histo[num];
	}
	/*
		First 4 moments of the gray-level image should match the first 4 moments
		of the target binary image. This leads to 4 equalities whose solutions
		are given in the Appendix of Ref. 1
		*/
	cd = m0 * m2 - m1 * m1;
	c0 = (-m2 * m2 + m1 * m3) / cd;
	c1 = (m0 * -m3 + m2 * m1) / cd;
	z0 = 0.5 * (-c1 - sqrt(c1 * c1 - 4.0 * c0));
	z1 = 0.5 * (-c1 + sqrt(c1 * c1 - 4.0 * c0));
	p0 = (z1 - m1) / (z1 - z0);  /* Fraction of the object pixels in the target binary image */

	sum = 0;
	for (num = 0; num < 256; num++) {
		sum += histo[num];
		if (sum > p0) {
			threshold = num;
			break;
		}
	}
	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);

	return output;
}

/*********************************************** minError *****************************************************/

int Mean(int* img)
{
	int threshold = -1;
	long tot = 0, sum = 0;
	for (int i = 0; i < 256; i++) {
		tot += img[i];
		sum += ((long)i * (long)img[i]);
	}
	threshold = (int)floor(sum / tot);
	return threshold;
}

double A(int* img, int stop)//�֥[graylevel �ƶq

{
	double x = 0;
	for (int i = 0; i <= stop; i++)
		x += img[i];
	return x;
}

double B(int* img, int stop)//�֥[graylevel * �ܼ�

{
	double x = 0;
	for (int i = 0; i <= stop; i++)
		x += (i * img[i]);
	return x;
}

double C(int* img, int stop)//�֥[graylevel * �ܼ� * �ܼ�
{
	double x = 0;
	for (int i = 0; i <= stop; i++)
		x += (i * i * img[i]);
	return x;
}

Mat minError(Mat& gray)
{

	cv::Mat output;

	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	for (num = 0; num < 256; num++)
		hist_use[num] = hist[num];
	int threshold = Mean(hist_use); //Initial estimate for the threshold is found with the MEAN algorithm.
	int Tprev = -2;
	double mu, nu, p, q, sigma2, tau2, w0, w1, w2, sqterm, temp;
	while (threshold != Tprev)
	{
		//Calculate some statistics.
		mu = B(hist_use, threshold) / A(hist_use, threshold);
		nu = (B(hist_use, 255) - B(hist_use, threshold)) / (A(hist_use, 255) - A(hist_use, threshold));
		p = A(hist_use, threshold) / A(hist_use, 255);
		q = (A(hist_use, 255) - A(hist_use, threshold)) / A(hist_use, 255);
		sigma2 = C(hist_use, threshold) / A(hist_use, threshold) - (mu * mu);
		tau2 = (C(hist_use, 255) - C(hist_use, threshold)) / (A(hist_use, 255) - A(hist_use, threshold)) - (nu * nu);

		//The terms of the quadratic equation to be solved.
		w0 = 1.0 / sigma2 - 1.0 / tau2;
		w1 = mu / sigma2 - nu / tau2;
		w2 = (mu * mu) / sigma2 - (nu * nu) / tau2 + log10((sigma2 * (q * q)) / (tau2 * (p * p)));

		//If the next threshold would be imaginary, return with the current one.
		sqterm = (w1 * w1) - w0 * w2;
		if (sqterm < 0) {
			cout << "MinError(I): not converging. Try \'Ignore black/white\' options" << endl;
			break;
		}

		//The updated threshold is the integer part of the solution of the quadratic equation.
		Tprev = threshold;
		temp = (w1 + sqrt(sqterm)) / w0;
		if (temp != temp) //�P�_�O�_���Ʀr ~ (Double.isNan())
		{
			cout << "MinError(I): NaN, not converging. Try \'Ignore black/white\' options" << endl;
			threshold = Tprev;
		}
		else

			threshold = (int)floor(temp);
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);

	return output;
}

/*********************************************** maxEntropy *****************************************************/

Mat maxEntropy(Mat& gray)
{
	cv::Mat output;
	int* hist_use = (int*)malloc(256 * sizeof(int));
	int* hist = getHist(gray);
	int num;
	for (num = 0; num < 256; num++) hist_use[num] = hist[num];
	double* P1 = (double*)malloc(256 * sizeof(double));
	double* P2 = (double*)malloc(256 * sizeof(double));
	double* hist_norm = (double*)malloc(256 * sizeof(double));
	long tol = gray.rows * gray.cols;
	int first_bin = 0;
	int stop = 0;
	for (num = 0; num < 256; num++)
	{
		hist_norm[num] = double(hist_use[num]) / tol;
		P1[num] = (!num) ? hist_norm[0] : (P1[num - 1] + hist_norm[num]); //num > 0 �|�֥[�e�@��
		P2[num] = (!num) ? (1.0 - P1[0]) : (1.0 - P1[num]); //num > 0 �|�֥[�e�@�� -->����
		/* Determine the first non-zero bin */
		if (!(abs(P1[num]) < 2.220446049250313E-16) && (stop == 0))
		{
			first_bin = num;
			stop++;
		}
	}
	/* Determine the last non-zero bin */
	int last_bin = 255;
	for (num = 255; num >= first_bin; num--) {
		if (!(abs(P2[num]) < 2.220446049250313E-16)) {
			last_bin = num;
			break;
		}
	}

	//cout << first_bin << " " << last_bin << endl;
	// Calculate the total entropy each gray-level and find the threshold that maximizes it 
	int threshold = -1;
	double min_ent = 256;
	double ent_back, ent_obj, tot_ent, max_ent;
	max_ent = 0;
	for (num = first_bin; num <= last_bin; num++)
	{
		ent_back = 0.0;
		for (int ih = 0; ih <= num; ih++)
		{
			if (hist_use[ih] != 0)
				ent_back -= (hist_norm[ih] / P1[num]) * log(hist_norm[ih] / P1[num]);

		}

		/* Entropy of the object pixels */
		ent_obj = 0.0;
		for (int ih = num + 1; ih < 256; ih++)
		{
			if (hist_use[ih] != 0)
				ent_obj -= (hist_norm[ih] / P2[num]) * log(hist_norm[ih] / P2[num]);

		}

		/* Total entropy */
		tot_ent = ent_back + ent_obj;

		//cout << tot_ent << "\n" << endl;
		if (max_ent < tot_ent) {
			max_ent = tot_ent;
			threshold = num;
		}
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);

	return output;
}

/*********************************************** isoData *****************************************************/

Mat isoData(Mat& gray)
{
	cv::Mat output;

	int* hist = getHist(gray);
	int num;
	int l, toth, totl, h, g = 0;
	for (num = 1; num < 256; num++)
	{
		//cout << hist[num] << endl;
		if (hist[num] > 0)
		{
			g = num + 1;
			break;
		}
	}
	while (true)
	{
		//cout << g << "\n" << endl;
		l = 0;
		totl = 0;
		for (num = 0; num < g; num++)
		{
			totl = totl + hist[num];
			l = l + (hist[num] * num);
		}
		h = 0;
		toth = 0;
		for (num = g + 1; num < 256; num++)
		{
			toth += hist[num];
			h += (hist[num] * num);
		}
		if (totl > 0 && toth > 0)
		{
			l /= totl;
			h /= toth;
			if (g == (int)round((l + h) / 2.0))
				break;
		}
		g++;
		if (g > 254)
		{
			cout << "IsoData Threshold not found." << endl;
			break;
		}
	}

	//std::cout << "threshold value:" << g << std::endl;

	output = binarize(gray, g);

	return output;

}

/*********************************************** huang *****************************************************/

Mat huang(Mat& gray)
{
	int threshold = -1, it, first_bin, last_bin, sum_pix, num_pix;
	double term;
	double ent;  // entropy 
	double min_ent; // min entropy 
	double mu_x;
	cv::Mat output;
	int* hist = getHist(gray);
	int num;
	long tol = gray.rows * gray.cols;

	/* Determine the first non-zero bin */
	first_bin = 0;
	for (num = 0; num < 256; num++)
	{
		if (hist[num] != 0) {
			first_bin = num;
			break;
		}
	}

	/* Determine the last non-zero bin */
	last_bin = 255;
	for (num = 255; num >= first_bin; num--)
	{
		if (hist[num] != 0)
		{
			last_bin = num;
			break;
		}
	}
	//cout << first_bin << " " << last_bin << endl;
	term = 1.0 / (double)(last_bin - first_bin);
	double* mu_0 = (double*)malloc(256 * sizeof(double));
	sum_pix = 0;
	num_pix = 0;
	for (num = first_bin; num < 256; num++)
	{
		sum_pix += num * hist[num];
		num_pix += hist[num];
		/* NUM_PIX cannot be zero ! */
		mu_0[num] = sum_pix / (double)num_pix;
	}

	double* mu_1 = (double*)malloc(256 * sizeof(double));
	sum_pix = 0;
	num_pix = 0;
	for (num = last_bin; num > 0; num--)
	{
		sum_pix += num * hist[num];
		num_pix += hist[num];
		/* NUM_PIX cannot be zero ! */
		mu_1[num - 1] = sum_pix / (double)num_pix;
	}

	/* Determine the threshold that minimizes the fuzzy entropy */
	min_ent = numeric_limits<double>::max();
	for (it = 0; it < 256; it++)
	{
		ent = 0.0;
		for (num = 0; num <= it; num++)
		{
			/* Equation (4) in Ref. 1 */
			mu_x = 1.0 / (1.0 + term * abs(num - mu_0[it]));

			if (!((mu_x < 1e-06) || (mu_x > 0.999999))) /* Equation (6) & (8) in Ref. 1 */
				ent += hist[num] * (-mu_x * log(mu_x) - (1.0 - mu_x) * log(1.0 - mu_x));
		}
		//cout << mu_x << " " << ent << endl;
		for (num = it + 1; num < 256; num++)
		{
			/* Equation (4) in Ref. 1 */
			mu_x = 1.0 / (1.0 + term * abs(num - mu_1[it]));
			if (!((mu_x < 1e-06) || (mu_x > 0.999999)))	/* Equation (6) & (8) in Ref. 1 */
				ent += hist[num] * (-mu_x * log(mu_x) - (1.0 - mu_x) * log(1.0 - mu_x));
		}

		//cout << ent << "\n" << endl;
		/* No need to divide by NUM_ROWS * NUM_COLS * LOG(2) ! */
		if (ent < min_ent)
		{
			min_ent = ent;
			threshold = it;
		}
	}

	//std::cout << "threshold value:" << threshold << std::endl;

	output = binarize(gray, threshold);


	return output;

}

/* reference : https://imagej.net/plugins/auto-threshold */
Mat huang2(Mat& gray)
{
	int first, last; 	// first and last non-empty bin
	int* hist = getHist(gray);

	for (first = 0; first < 256 && !hist[first]; ++first)
		; // do nothing
	for (last = 255; last > first && !hist[last]; --last)
		; // do nothing
	if (first == last)
		return binarize(gray, 0);

	// calculate the cumulative density and the weighted cumulative density
	double* S = new double[last + 1];
	double* W = new double[last + 1];

	S[0] = hist[0];
	W[0] = 0;
	for (int i = max(1, first); i <= last; i++)
	{
		S[i] = S[i - 1] + hist[i];
		W[i] = W[i - 1] + i * hist[i];
	}

	// precalculate the summands of the entropy given the absolute difference x - mu (integral)
	double C = last - first;
	double* Smu = new double[last + 1 - first];
	for (int i = 1; i < last + 1 - first; ++i)
	{
		double mu = 1 / (1 + i / C);
		Smu[i] = -mu * log(mu) - (1 - mu) * log(1 - mu);
	}

	// calculate the threshold
	int bestThreshold = 0;
	double bestEntropy = std::numeric_limits<double>::max();
	for (int threshold = first; threshold <= last; threshold++)
	{
		double entropy = 0;
		int mu = (int)round(W[threshold] / S[threshold]);
		for (int i = first; i <= threshold; ++i)
			entropy += Smu[abs(i - mu)] * hist[i];

		mu = (int)round((W[last] - W[threshold]) / (S[last] - S[threshold]));
		for (int i = threshold + 1; i <= last; ++i)
			entropy += Smu[abs(i - mu)] * hist[i];

		if (bestEntropy > entropy)
		{
			bestEntropy = entropy;
			bestThreshold = threshold;
		}
	}
	return binarize(gray, bestThreshold);
}