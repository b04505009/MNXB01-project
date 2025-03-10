/*
 *     Creator: Xi-Zhen Liu
 *     Email:   xi2645li-s@student.lu.se
 *
 *
 * Description: 
 *     This script takes an input file, and print the year and the hottest and coldest date. Design for input format of Uppsala.
 * 
 * 
 * Usage:
 *     ./dateOfPeakTemp [mode]
 *     For example:
 *     ./dateOfPeakTemp 1
 *     Mode:
 *         1: Daily average temperature according to observations. 
 *         2: Daily average temperatures corrected for the urban effect.  
 *
 *
 * Methods:  
 *
 *     * Gaussian(double *x, double *par)
 *       - Description:
 *         * Gaussian function used for fitting histograms
 * 
 *     * peak_temp_Upp_old(ifstream &inFile, int start_year, int mode, int location_specified)
 *       - Description:
 *         * Original version for plotting histogram. Deprecated.
 *       - Arguments needed: 
 *         * inFile: input file
 *         * start_year: The year the data start
 *         * mode: 
 *               1: Daily average temperature according to observations. 
 *               2: Daily average temperatures corrected for the urban effect.
 *         * location_specified:
 *               0: Not speficied
 *               1: Uppsala
 *               2: Risinge
 *               3: Betna
 *               4: Linköping
 *               5: Stockholm
 *               6: Interpolated
 *
 * 
 *     * peak_temp_Upp(ifstream &inFile, int start_year, int mode, int location_specified)
 *       - Description:
 *         * In this version, we only use coldest day which is less than 100 or more than 300 in day of the year.
 *           Similarly, we use hottest day which is between 100 and 300 in day of the year.
 *           We extend the histogram to start from -182 and end at 549, in order to make the fitting more reasonable.
 *
 *       - Arguments needed: 
 *         * inFile: input file
 *         * start_year: The year the data start
 *         * mode: 
 *               1: Daily average temperature according to observations. 
 *               2: Daily average temperatures corrected for the urban effect.
 *         * location_specified:
 *               0: Not speficied
 *               1: Uppsala
 *               2: Risinge
 *               3: Betna
 *               4: Linköping
 *               5: Stockholm
 *               6: Interpolated
 *
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <utility>
#include "Date.cpp"
#include "TH1I.h"
#include "TLegend.h"
#include "TF1.h"
#include "TCanvas.h"
#include <TROOT.h>
#include <TStyle.h>
using namespace std;


// Gaussian function used for fitting histograms
double Gaussian(double *x, double *par)
{
    return par[0] * exp(-0.5 * (x[0] * x[0] - 2 * x[0] * par[1] + par[1] * par[1]) / (par[2] * par[2]));
}

/* 
    Plot histogram for hottest and coldest date in Uppsala.
    Parameters:
        inFile: input file
        start_year: The year the data start
        mode: 
            1: Daily average temperature according to observations. 
            2: Daily average temperatures corrected for the urban effect.
        location_specified:
            0: Not speficied
            1: Uppsala
            2: Risinge
            3: Betna
            4: Linköping
            5: Stockholm
            6: Interpolated

 */
void peak_temp_Upp(ifstream &inFile, int start_year, int mode, int location_specified)
{
	gStyle->SetOptFit(0);
    int year = 0;
    int month = 0;
    int day = 0;
    double temp1 = 0;
    double temp2 = 0;
    double temp = 0;
    int location = 0;
    Date hottest = Date();
    Date coldest = Date();

    vector<int> num_hot(366);
    vector<int> num_cold(366);

    TH1I *hist_cold_1 = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, -182, 184);
    TH1I *hist_cold_2 = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 183, 549);
    TH1I *hist_hot = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 1, 367);

    hist_cold_1->SetFillColor(kBlue - 7);
    hist_cold_1->SetLineColor(kBlue - 7);
    hist_cold_2->SetFillColor(kBlue - 7);
    hist_cold_2->SetLineColor(kBlue - 7);
    hist_hot->SetFillColor(kRed + 1);
    hist_hot->SetLineColor(kRed + 1);

    while (!inFile.eof())
    {
        inFile >> year >> month >> day >> temp1 >> temp2 >> location;
        //cout << year<< month <<day << temp1 << temp2 << location<< endl;
        if (location == location_specified || location_specified == 0)
        {
            if (year > start_year)
            {
                //cout << "Coldest temperature for " << start_year << " year is " << coldest.get_temp() << ", which is at " << coldest.to_string() << ", day of year " << coldest.day_of_year() << "\n";
                //cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
                //num_cold.at(coldest.day_of_year()-1)++;
                if (hottest.day_of_year() > 100 && hottest.day_of_year() < 300)
                    hist_hot->Fill(hottest.day_of_year());
                
                if (coldest.day_of_year() < 100)
                {
                    hist_cold_1->Fill(coldest.day_of_year());
                    hist_cold_2->Fill(coldest.day_of_year() + 366);
                }
                if (coldest.day_of_year() > 300)
                {
                    hist_cold_1->Fill(coldest.day_of_year() - 366);
                    hist_cold_2->Fill(coldest.day_of_year());
                }
                //num_hot.at(hottest.day_of_year()-1)++;
                start_year = year;
                hottest = Date(year, month, day);
                coldest = Date(year, month, day);
            }
            if (mode == 1)
                temp = temp1;
            else
                temp = temp2;
            Date d = Date(year, month, day);
            if (temp > hottest.get_temp())
            {
                hottest = d;
                hottest.set_temp(temp);
            }
            if (temp < coldest.get_temp())
            {
                coldest = d;
                coldest.set_temp(temp);
            }
        }
    }
    //cout << "Coldest temperature for " << start_year << " year is " << coldest.get_temp() << ", which is at " << coldest.to_string() << ", day of year " << coldest.day_of_year() << "\n";
    //cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
    if (hottest.day_of_year() > 100 && hottest.day_of_year() < 300)
        hist_hot->Fill(hottest.day_of_year());
    if (coldest.day_of_year() < 100)
        hist_cold_1->Fill(coldest.day_of_year());
    hist_cold_2->Fill(coldest.day_of_year() + 366);
    if (coldest.day_of_year() > 300)
        hist_cold_1->Fill(coldest.day_of_year() - 366);
    hist_cold_2->Fill(coldest.day_of_year());
    //num_cold.at(coldest.day_of_year()-1)++;
    //num_hot.at(hottest.day_of_year()-1)++;
    TF1 *func_1 = new TF1("Gaussian", Gaussian, 1, 366, 3);
    TF1 *func_2 = new TF1("Gaussian", Gaussian, 1, 366, 3);
    TF1 *func_3 = new TF1("Gaussian", Gaussian, 1, 366, 3);

    func_1->SetParameters(5, 20, 50); //Starting values for fitting
    func_1->SetLineColor(kBlack);
    func_1->SetLineStyle(2);
    func_2->SetParameters(5, 200, 50); //Starting values for fitting
    func_2->SetLineColor(kBlack);
    func_3->SetParameters(5, 386, 50); //Starting values for fitting
    func_3->SetLineColor(kBlack);
    func_3->SetLineStyle(2);
    hist_cold_1->Fit(func_1, "QR", "", -182, 184);
    hist_hot->Fit(func_2, "QR", "", 1, 366);
    hist_cold_2->Fit(func_3, "+QR", "", 183, 549);

    TLegend *leg = new TLegend(0.65, 0.75, 0.92, 0.92, "", "NDC");
    leg->SetFillStyle(0);                           //Hollow fill (transparent)
    leg->SetBorderSize(0);                          //Get rid of the border
    leg->AddEntry(hist_cold_1, "Coldest day", "F"); //Use object title, draw fill
    leg->AddEntry(hist_hot, "Warmest day", "F");    //Use custom title

    // plot histogram for first cold section, range(-182, 184)
    TCanvas *can = new TCanvas();
    hist_cold_1->Draw();
    can->SaveAs("../images/hotCold_Upp_cold_1.pdf");
    can->Close();
    // plot histogram for second cold section, range(183, 549)
    can = new TCanvas();
    hist_cold_2->Draw();
    can->SaveAs("../images/hotCold_Upp_cold_2.pdf");
    can->Close();
    // plot histogram for hot section, range(1, 366)
    can = new TCanvas();
    hist_hot->Draw();
    can->SaveAs("../images/hotCold_Upp_hot.pdf");
    can->Close();

    hist_cold_1->GetListOfFunctions()->Remove(func_1);
    hist_cold_2->GetListOfFunctions()->Remove(func_3);
    // plot histogram for whole year (1, 366), and combined those 3 plots and fit lines
    hist_cold_1->Fit(func_1, "QR", "", 1, 200);
    hist_hot->Fit(func_2, "QR", "", 1, 366);
    hist_cold_2->Fit(func_3, "QR", "", 200, 366);
    cout << "The mean for Warmest is " << func_2->GetParameter(1) << endl;
    cout << "Its uncertainty for Warmest is " << func_2->GetParError(1) << endl;
    
    can = new TCanvas();
    hist_hot->Draw();
    hist_cold_1->Draw("SAME");
    hist_cold_2->Draw("SAME");
    leg->Draw("SAME");
    can->SaveAs("../images/hotCold_Upp_final.pdf");
    return;
}
// Original version for plotting histogram. Deprecated.
void peak_temp_Upp_old(ifstream &inFile, int start_year, int mode, int location_specified)
{
    int year = 0;
    int month = 0;
    int day = 0;
    double temp1 = 0;
    double temp2 = 0;
    double temp = 0;
    int location = 0;
    Date hottest = Date();
    Date coldest = Date();

    vector<int> num_hot(366);
    vector<int> num_cold(366);

    TH1I *hist_cold = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 1, 367);
    TH1I *hist_hot = new TH1I("Hottest and Coldest", "Day;Day of year;Entries", 366, 1, 367);

    hist_cold->SetFillColor(kBlue - 7);
    hist_cold->SetLineColor(kBlue - 7);
    hist_hot->SetFillColor(kRed + 1);
    hist_hot->SetLineColor(kRed + 1);

    while (!inFile.eof())
    {
        inFile >> year >> month >> day >> temp1 >> temp2 >> location;
        //cout << year<< month <<day << temp1 << temp2 << location<< endl;
        if (location == location_specified || location_specified == 0)
        {
            if (year > start_year)
            {
                if (hottest.day_of_year() < 100)
                {
                    cout << "Hottest temperature for " << start_year << " year is " << hottest.get_temp() << ", which is at " << hottest.to_string() << ", day of year " << hottest.day_of_year() << "\n";
                }
                hist_cold->Fill(coldest.day_of_year());
                hist_hot->Fill(hottest.day_of_year());
                start_year = year;
                hottest = Date(year, month, day);
                coldest = Date(year, month, day);
            }
            if (mode == 1)
                temp = temp1;
            else
                temp = temp2;
            if (temp > hottest.get_temp())
            {
                hottest = Date(year, month, day);
                hottest.set_temp(temp);
            }
            if (temp < coldest.get_temp())
            {
                coldest = Date(year, month, day);
                coldest.set_temp(temp);
            }
        }
    }
    hist_cold->Fill(coldest.day_of_year());
    hist_hot->Fill(hottest.day_of_year());

    TF1 *func_1 = new TF1("Gaussian", Gaussian, 1, 366, 3);
    TF1 *func_2 = new TF1("Gaussian", Gaussian, 1, 366, 3);
    TF1 *func_3 = new TF1("Gaussian", Gaussian, 1, 366, 3);
    func_1->SetParameters(1, 100, 50); //Starting values for fitting
    func_1->SetLineColor(kBlack);
    func_1->SetLineStyle(2);
    func_2->SetParameters(100, 300, 50); //Starting values for fitting
    func_2->SetLineColor(kBlack);
    func_3->SetParameters(300, 366, 50); //Starting values for fitting
    func_3->SetLineColor(kBlack);
    func_3->SetLineStyle(2);
    hist_cold->Fit(func_1, "QR");
    hist_hot->Fit(func_2, "QR");
    hist_cold->Fit(func_3, "QR+");
    cout << "The mean for Warmest is " << func_2->GetParameter(1) << endl;
    cout << "Its uncertainty for Warmest is " << func_2->GetParError(1) << endl;
    TLegend *leg = new TLegend(0.65, 0.75, 0.92, 0.92, "", "NDC");
    leg->SetFillStyle(0);                         //Hollow fill (transparent)
    leg->SetBorderSize(0);                        //Get rid of the border
    leg->AddEntry(hist_cold, "Coldest day", "F"); //Use object title, draw fill
    leg->AddEntry(hist_hot, "Warmest day", "F");  //Use custom title
    TCanvas *can = new TCanvas();
    hist_cold->Draw();
    hist_hot->Draw("SAME");
    leg->Draw();
    can->SaveAs("../image/hotCold_Upp_prev.pdf");
    return;
}
/*
// Use as standalone execution
int main(int argc, char *argv[])
{
    int chosen_year;
    int number_of_columns = 6;
    int temp_col = 4;
    double av = 0;
    int start_year = 1722;
    int location_specified = 1;
    int mode = strtol(argv[1], NULL, 10);
    //Choose path to file
    std::ifstream inFile("CleanData/datasets/uppsala_tm_1722-2013.dat");
    if (!inFile)
    {
        std::cout << "Not in file";
        return 1;
    }
    peak_temp_Upp(inFile, start_year, mode, location_specified);

    return 0;
}
*/
