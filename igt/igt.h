#pragma once
// clang-format off
#include <windows.h>
// clang-format on
#include <shellapi.h>
#include <winuser.h>

#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>
#include<fstream>

#include "framework.h"
#include "resource.h"

#include <stopwatch.h>
#include <utils.h>

enum class eStages { WELCOME, INTRODUCTION, INTASK, ENDMSG, END, LAST };
enum class tStages { ITI, SHOWDECKS, FEEDBACK, LAST };

/**
 * @brief buttons serving as IGT decks
 * 
*/
class IGTButton {
  HWND hw_;
  int x_;
  int y_;
  int w_;
  int h_;
  std::string label_;

public:
  IGTButton(HWND hw_p, std::string label, int x, int y, int w, int h);
  HWND getHandle() { return hw_; }
  void show();
  void hide();
  bool Contains(int x, int y);
  std::string getLabel();
};

/**
 * @brief read experiment parameters, instructions, and end message from files
 *
 * parameter file contains 9 lines. first four lines contain deck parameters.
 * line 5 contains iti (s) double, line 6 contains fbduration (s) double, line 7
 * contains int number of trials, line 8 contains double starting balance, line
 * 9 contain string currency label.
 *
 * //clang format off
 * deck parameter lines:
 * label £win pwin £lose plose
 * char double double double double
 *
 * e.g.
 * A    1   0.5     2   0.7
 * //clang format on
 *
 * given a parameter file params.txt also need an instructions file
 * params.txt_Inst and a file for end message param.txt_EndMsg. These should be
 * short text files. Enter each paragraph on a separate line.
 */
class ExptParams {
  bool OK = false;
  int id_;
  std::string f_;
  std::map<std::string, std::tuple<double, double, double, double>> params;
  std::vector<std::string> labels;
  int nt;
  double iti;        // sec
  double fbDuration; // sec
  double balance;
  std::string currency;
  std::string instructions;
  std::string endMsg;

public:
  ExptParams() : id_(0), nt(0), iti(0), fbDuration(0), balance(0){};
  ExptParams(std::wstring f, int id);
  bool isOK() { return OK; }
  std::tuple<double, double, double, double> getParams(std::string id) const;

  /**
   * @brief labels for buttons (from param file)
   * @return 
  */
  std::vector<std::string> getButtonIDLabels() const { return labels; }

  /**
   * @brief number of trials
   * @return 
  */
  int getNT() const { return nt; }
  /**
   * @brief participant ID
   * @return 
  */
  int getID() { return id_; }
  double getITI() { return iti; }
  /**
   * @brief feedback duration
   * @return 
  */
  double getFBD() { return fbDuration; }
  double getStartBalance() const { return balance; }
  std::string getCurrency() const { return currency; }
  std::string getInstructions() { return instructions; }
  std::string getEndMsg() { return endMsg; }
  std::string getDataFN() { return f_ + "_Data"; }
};

/**
 * @brief add experiment messages to message map mm and rects to mr
 */
void initMessages(std::map<std::string, std::string> &mm,
                  std::map<std::string, LPRECT> &mr);

/**
 * @brief initialise global coordinates
 */
void initCoords(RECT cr);

/**
 * @brief initialise deck buttons
 */
void initButtons(HWND hWnd, std::vector<IGTButton> &buttons, ExptParams const & p);
/**
 * @brief initialise global vars
 * 
*/
void initVars(ExptParams const &p);
void hideButtons(std::vector<IGTButton> &buttons);
void showButtons(std::vector<IGTButton> &buttons);

/**
 * @brief find the button by hWnd
 */
std::string getButton(HWND hWnd, std::vector<IGTButton> &buttons);

std::wstring getwstr(std ::string s) { return {s.begin(), s.end()}; }
std::string getstr(std::wstring s) { return {s.begin(), s.end()}; }

/**
 * @brief increment an enum class looping back to start if they hit LAST
 */
template <class T> T increment(T stage) {
  auto buff = spgxyz::to_underlying(stage);
  buff++;
  T result;
  if (buff == spgxyz::to_underlying(T::LAST))
    result = static_cast<T>(0);
  else
    result = static_cast<T>(buff);
  return result;
}

/**
 * @brief terminates the experiment
 *
 */
void closeDown();

/**
 * @brief main experiment logic
 * @return false to end
 */
bool experiment();

void clearScreen(HWND hw, HBRUSH hb);
void showTrial(HWND hw);
/**
 * @brief 
 * @param bl 
*/
std::pair<double,double> getFeedBack(std::string bl, ExptParams const & ep);

std::string fmt(double a, ExptParams const &ep);

struct TData {
  int ID;
  int TN;
  char deck;
  double win;
  double loss;
  double net;
  double balance;
  std::string dts;
};

void writeData(std::string f, std::vector<TData> const &data);
