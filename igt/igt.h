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

#include "framework.h"
#include "resource.h"

#include <stopwatch.h>
#include <utils.h>

enum class eStages { WELCOME, INTRODUCTION, INTASK, ENDMSG, END, LAST };
enum class tStages { ITI, SHOWDECKS, FEEDBACK, LAST };

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
 * @brief read experiment parameters from file
 *
 * file contains 7 lines. first four lines contain deck parameters. line 5
 * contains iti(s) double, line 6 contains fbduration(s) double, line 7 contains
 * int number of trials. 
 *
 * deck parameter lines: label £win pwin £lose plose char
 * double double double double
 *
 * e.g.
 * A    1   0.5     2   0.7
 */
class ExptParams {
  bool OK = false;
  int id_;
  std::string f_;
  std::map<std::string, std::tuple<double, double, double, double>> params;
  int nt;
  double iti;        // sec
  double fbDuration; // sec
  std::string instructions;

public:
  ExptParams() = default;
  ExptParams(std::wstring f, int id);
  bool isOK() { return OK; }
  std::tuple<double, double, double, double> getParams(std::string id) {
    return params[id];
  }
  int getNT() { return nt; }
  int getID() { return id_; }
  double getITI() { return iti; }
  double getFBD() { return fbDuration; }
  std::string getInstructions() { return instructions; }
};

/**
 * add experiment messages to message map mm and rects to mr
 */
void initMessages(std::map<std::string, std::string> &mm,
                  std::map<std::string, LPRECT> &mr);

/**
 * initialise global coordinates
 */
void initCoords(RECT cr);

/**
 * initialise deck buttons
 */
void initButtons(HWND hWnd, std::vector<IGTButton> &buttons);

void hideButtons(std::vector<IGTButton> &buttons);
void showButtons(std::vector<IGTButton> &buttons);

/**
 * find the button by hWnd
 */
std::string getButton(HWND hWnd, std::vector<IGTButton> &buttons);

std::wstring getwstr(std ::string s) { return {s.begin(), s.end()}; }
std::string getstr(std::wstring s) { return {s.begin(), s.end()}; }

/**
 * increment an enum class looping back to start if they hit LAST
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

/**
 * @brief update screen
 */
void render();

void clearScreen(HWND hw, HBRUSH hb);
void showTrial(HWND hw);
