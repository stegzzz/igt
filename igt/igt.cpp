// igt.cpp : Defines the entry point for the application.
//

#include "igt.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                     // current instance
WCHAR szTitle[MAX_LOADSTRING];       // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

std::vector<IGTButton> buttons;
std::vector<TData> results;
TData g_TData;
int g_w;
int g_h;
RECT g_MsgRect1;
RECT g_MsgRect2;
RECT g_MsgRect3;
RECT g_MsgRect4;
std::string cfgFile;
int participantID;
bool paramCheck = false;
ExptParams params;

std::map<std::string, std::string> messages;
std::map<std::string, LPRECT> msgRects;

eStages ESTAGE;
tStages TSTAGE;
bool experimentRunning;

char g_bpl; // global button state

HWND g_hWnd = nullptr;
spgxyz::stopwatch segmentTimer;
spgxyz::MyMTRNG g_mtrng;
auto g_rng = g_mtrng.getRNG();
std::uniform_real_distribution g_urd{0.0, 1.0};

COLORREF gray;
COLORREF white;
COLORREF bkgnd;

HBRUSH g_brush;
HBRUSH g_wbrush;

int g_TrialCounter;
double g_balance;
std::string g_currency;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.

  // Initialize global strings
  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_IGT, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, SW_SHOWMAXIMIZED)) {
    return FALSE;
  }

  if (!params.isOK()) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
  }
  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IGT));

  MSG msg;

  experimentRunning = true;
  // Main message loop:
  // https://stackoverflow.com/questions/1800250/is-there-a-better-way-to-create-this-game-loop-c-windows/1800466
  while (experimentRunning) {
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT || msg.message == WM_CLOSE ||
          msg.message == WM_DESTROY)
        break;
      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    } else {
      if (!experiment()) {
        closeDown();
        break;
      }
    }
  }

  return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IGT));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_IGT);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  hInst = hInstance; // Store instance handle in our global variable

  HWND hWnd =
      CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME, 0,
                   0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
  auto hMenu =GetMenu(hWnd);
  DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);//https://cboard.cprogramming.com/windows-programming/5724-disabling-window-x-button-please-help.html
  LONG style = GetWindowLong(hWnd, GWL_STYLE);
  style ^= WS_SYSMENU;
  SetWindowLong(hWnd, GWL_STYLE, style);

  if (!hWnd) {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  RECT rect;
  GetClientRect(hWnd, &rect);
  initCoords(rect);
  initMessages(messages, msgRects);
  initButtons(hWnd, buttons, params);

  UpdateWindow(hWnd);

  g_hWnd = hWnd;

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  switch (message) {

  case WM_CREATE: {
    
    int narg;
    auto cla = CommandLineToArgvW(GetCommandLine(), &narg);
    if (narg != 3) {
      auto msg = L"needs cfg file name and participant ID parameters!";
      CreateWindow(L"BUTTON", msg,
                   WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                   (g_w / 4), 50, g_w / 2, g_h / 4, hWnd, NULL,
                   (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
      break;
    }

    params = std::move(ExptParams{cla[1], std::stoi(cla[2])});
    if (!params.isOK()) {
      throw std::runtime_error{"experiment parameters not initialised"};
      closeDown();
    }
    initVars(params);
    break;
  }
  case WM_COMMAND: {
    int wmId = LOWORD(wParam);
    // Parse the menu selections:
    switch (wmId) {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      return 0;
    case IDM_EXIT:
      // DestroyWindow(hWnd);
      return 0;
      // default:
      //  return DefWindowProc(hWnd, message, wParam, lParam);
    }
    int hwwp = HIWORD(wParam);
    switch (hwwp) {
    case BN_CLICKED:
      if (TSTAGE == tStages::SHOWDECKS)
        g_bpl = getButton((HWND)lParam, buttons).at(0);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  } break;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    EndPaint(hWnd, &ps);
  } break;
  case WM_KEYDOWN:
    if ((wParam == 0x51) && GetKeyState(VK_CONTROL))
      closeDown();
    return 0;
  case WM_DESTROY:
    //PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}

IGTButton::IGTButton(HWND hw_p, std::string label, int x, int y, int w, int h)
    : x_(x), y_(y), w_(w), h_(h), label_(label) {
  std::wstring wn(label.begin(), label.end());
  hw_ = CreateWindow(L"BUTTON", wn.c_str(),
                     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, x,
                     y, w, h, hw_p, NULL,
                     (HINSTANCE)GetWindowLongPtr(hw_p, GWLP_HINSTANCE), NULL);
}

bool IGTButton::Contains(int x, int y) {
  if ((x < x_) || (y < y_))
    return false;
  if ((x > x_ + w_) || y > y_ + h_)
    return false;
  return true;
}

void IGTButton::show() { ShowWindow(hw_, SW_SHOW); }
void IGTButton::hide() { ShowWindow(hw_, SW_HIDE); }
std::string IGTButton::getLabel() { return label_; }

ExptParams::ExptParams(std::wstring f, int id)
    : id_(id), f_(getstr(f)), iti(0), fbDuration(0), balance(0) {

  try {
    auto lines = spgxyz::readFile(f_); // read parameters
    if (lines.size() != 9)
      throw std::exception{"need 9 lines"};
    // process parameters
    for (int i = 0; i < 4; ++i) { // four deck lines
      auto split = spgxyz::splitStr(lines[i], "\\s+");
      if (split.size() != 5)
        throw std::exception{"need 5 valid fields in deck specification"};
      if (split[0].length() != 1)
        throw std::exception{"deck label must be single character"};
      labels.push_back(split[0]);
      params.emplace(split[0],
                     std::make_tuple(std::stod(split[1]), std::stod(split[2]),
                                     std::stod(split[3]), std::stod(split[4])));
    } // for
    iti = std::stod(lines[4]);
    fbDuration = std::stod(lines[5]);
    nt = std::stoi(lines[6]);
    balance = std::stod(lines[7]);
    currency = lines[8];

  } catch (std ::exception e) {
    MessageBox(NULL, getwstr(std::string{"Error! "} + e.what()).c_str(),
               getwstr("Could not open/process file: " + f_).c_str(),
               MB_SYSTEMMODAL | MB_ICONSTOP | MB_SETFOREGROUND);
    OK = false;
    return;
  }

  try {
    auto ilines = spgxyz::readFile(f_ + "_Inst"); // read instructions
    for (auto s : ilines)
      instructions.append(s + "\n\n");
  } catch (std ::exception e) {
    MessageBox(NULL, getwstr(std::string{"Error! "} + e.what()).c_str(),
               getwstr("Could not open/process file: " + f_).c_str(),
               MB_SYSTEMMODAL | MB_ICONSTOP | MB_SETFOREGROUND);
    OK = false;
    return;
  }

  try {
    auto elines = spgxyz::readFile(f_ + "_EndMsg"); // read endmsg
    for (auto s : elines)
      endMsg.append(s + "\n\n");
  } catch (std ::exception e) {
    MessageBox(NULL, getwstr(std::string{"Error! "} + e.what()).c_str(),
               getwstr("Could not open/process file: " + f_).c_str(),
               MB_SYSTEMMODAL | MB_ICONSTOP | MB_SETFOREGROUND);
    OK = false;
    return;
  }

  if (id_ >= 0)
    OK = true; // if file opened,read and ID>=0
}

std::tuple<double, double, double, double>
ExptParams::getParams(std::string id) const {
  auto it = params.find(id);
  if (it != params.end())
    return it->second;
  else
    throw std::runtime_error{std::string{"invalid id passed into "} + __func__};
}

void initMessages(std::map<std::string, std::string> &mm,
                  std::map<std::string, LPRECT> &mr) {
  mm.emplace(std::make_pair("clkMsg", "Click on a deck to earn"));
  mr.emplace(std::make_pair("clkMsg", &g_MsgRect1));
  mm.emplace(std::make_pair("inst", "Please read these instructions: "));
  mr.emplace(std::make_pair("inst", &g_MsgRect1));
  mm.emplace(std::make_pair("welcome", "Welcome to the gambling task"));
  mr.emplace(std::make_pair("welcome", &g_MsgRect1));
  mm.emplace(
      std::make_pair("over", "Experiment over, please call the experimenter "));
  mr.emplace(std::make_pair("over", &g_MsgRect1));

  mm.emplace(std::make_pair("bal", "Your balance is: "));
  mr.emplace(std::make_pair("bal", &g_MsgRect3));
  mm.emplace(std::make_pair("yw", "You win: "));
  mr.emplace(std::make_pair("yw", &g_MsgRect2));
  mm.emplace(std::make_pair("yl", "You lose: "));
  mr.emplace(std::make_pair("yl", &g_MsgRect2));
  mm.emplace(std::make_pair("be", "Break even! "));
  mr.emplace(std::make_pair("be", &g_MsgRect2));
  mm.emplace(std::make_pair("tr", "Trials remaining: "));
  mr.emplace(std::make_pair("tr", &g_MsgRect4));

  mm.emplace(std::make_pair("cc", "click to continue"));
}

void initCoords(RECT rect) {
  g_w = rect.right - rect.left;
  g_h = rect.bottom - rect.top;

  g_MsgRect1.left = g_w / 4;
  g_MsgRect1.right = g_w - (g_MsgRect1.left);
  g_MsgRect1.top = g_h / 6;
  g_MsgRect1.bottom = (2 * g_h) / 6;

  g_MsgRect2.left = g_w / 4;
  g_MsgRect2.right = (g_w / 2) - 10;
  g_MsgRect2.top = (4 * g_h) / 6;
  g_MsgRect2.bottom = (5 * g_h) / 6;

  g_MsgRect3.left = (g_w / 2) + 10;
  g_MsgRect3.right = g_w - (g_w / 4);
  g_MsgRect3.top = (8 * g_h) / 12;
  g_MsgRect3.bottom = (9 * g_h) / 12;

  g_MsgRect4.left = (g_w / 2) + 10;
  g_MsgRect4.right = g_w - (g_w / 4);
  g_MsgRect4.top = (10 * g_h) / 12;
  g_MsgRect4.bottom = (11 * g_h) / 12;
}

void initButtons(HWND hWnd, std::vector<IGTButton> &buttons,
                 ExptParams const &p) {
  int bw = g_w / 9;
  auto labels = p.getButtonIDLabels();
  buttons.clear();
  buttons.push_back({hWnd, labels[0], bw, g_h / 3, bw, bw});
  buttons.push_back({hWnd, labels[1], 3 * bw, g_h / 3, bw, bw});
  buttons.push_back({hWnd, labels[2], 5 * bw, g_h / 3, bw, bw});
  buttons.push_back({hWnd, labels[3], 7 * bw, g_h / 3, bw, bw});
  hideButtons(buttons);
}

void initVars(ExptParams const &p) {
  g_TrialCounter = 0;
  g_balance = p.getStartBalance();
  g_currency = p.getCurrency();
  ESTAGE = eStages::WELCOME;
  TSTAGE = tStages::ITI;
  experimentRunning = false;
  gray = RGB(192, 192, 192);
  white = RGB(255, 255, 255);
  bkgnd = white;
  g_brush = CreateSolidBrush(gray);
  g_wbrush = CreateSolidBrush(white);
  g_bpl = 0;
}

void hideButtons(std::vector<IGTButton> &buttons) {
  for (auto &b : buttons)
    b.hide();
}
void showButtons(std::vector<IGTButton> &buttons) {
  for (auto &b : buttons)
    b.show();
}

void closeDown() {
  ESTAGE = eStages::END;
  experimentRunning = false;
  writeData(params.getDataFN(), results);
  PostQuitMessage(0);
}

std::string getButton(HWND hWnd, std::vector<IGTButton> &buttons) {
  for (auto b : buttons) {
    if (b.getHandle() == hWnd)
      return b.getLabel();
  }
  return "";
}

bool experiment() {
  switch (ESTAGE) {
  case eStages::WELCOME:
    MessageBox(NULL, getwstr(messages["welcome"]).c_str(),
               getwstr(messages["cc"]).c_str(),
               MB_SYSTEMMODAL | MB_OK | MB_SETFOREGROUND);
    ESTAGE = spgxyz::increment<eStages>(ESTAGE);
    break;
  case eStages::INTRODUCTION:
    MessageBox(NULL, getwstr(params.getInstructions()).c_str(),
               getwstr(messages["cc"]).c_str(),
               MB_SYSTEMMODAL | MB_OK | MB_SETFOREGROUND);

    ESTAGE = spgxyz::increment<eStages>(ESTAGE);
    clearScreen(g_hWnd, g_brush);
    segmentTimer.start(); // start with an ITI

    break;
  case eStages::INTASK:

    switch (TSTAGE) {
    case tStages::ITI: {
      segmentTimer.stop();
      if (segmentTimer.read() > params.getITI()) {
        showTrial(g_hWnd);
        HDC hdc = GetDC(g_hWnd);
        auto buff = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, getwstr(messages["bal"] + fmt(g_balance, params)).c_str(),
                 -1, msgRects["bal"], DT_CENTER || DT_TOP);
        DrawText(hdc,
                 getwstr(messages["tr"] +
                         std::to_string(params.getNT() - g_TrialCounter))
                     .c_str(),
                 -1, msgRects["tr"], DT_CENTER || DT_TOP);
        SetBkMode(hdc, buff);
        ReleaseDC(g_hWnd, hdc);
        TSTAGE = spgxyz::increment<tStages>(TSTAGE);
        g_TData.ID = params.getID();
        g_TData.TN = g_TrialCounter;
      } else
        segmentTimer.resume();
    } break;
    case tStages::SHOWDECKS:
      if (g_bpl) { // a deck has been clicked, give feedback
        InvalidateRect(g_hWnd, msgRects["bal"], true);
        UpdateWindow(g_hWnd);
        HDC hdc = GetDC(g_hWnd);
        auto buff = SetBkMode(hdc, TRANSPARENT);
        auto [win, lose] = getFeedBack(std::string{g_bpl}, params);
        auto net = win - lose;
        if (net > 0)
          DrawText(hdc,
                   getwstr("\n" + messages["yw"] + fmt(net, params)).c_str(),
                   -1,
                   msgRects["yw"], DT_CENTER || DT_TOP);
        if (net < 0)
          DrawText(hdc,
                   getwstr("\n" + messages["yl"] + fmt(std::abs(net), params))
                       .c_str(),
                   -1, msgRects["yl"], DT_CENTER || DT_TOP);
        if (net == 0)
          DrawText(hdc, getwstr("\n" + messages["be"]).c_str(), -1,
                   msgRects["be"],
                   DT_CENTER || DT_TOP);
        g_balance += net;

        DrawText(hdc,
                 getwstr("\n"+ messages["bal"] + fmt(g_balance, params)).c_str(),
                 -1, msgRects["bal"], DT_CENTER || DT_TOP);
        SetBkMode(hdc, buff);
        ReleaseDC(g_hWnd, hdc);

        g_TData.deck = g_bpl;
        g_TData.win = win;
        g_TData.loss = lose;
        g_TData.net = net;
        g_TData.balance = g_balance;
        g_TData.dts = spgxyz::DTS();
        results.push_back(g_TData);
        g_bpl = 0;

        segmentTimer.stop();
        segmentTimer.start(); // start feedback duration timer

        TSTAGE = spgxyz::increment<tStages>(TSTAGE);
      }
      break;
    case tStages::FEEDBACK:
      segmentTimer.stop();
      if (segmentTimer.read() > params.getFBD()) {
        hideButtons(buttons);
        clearScreen(g_hWnd, g_brush);
        g_TrialCounter++;
        if (g_TrialCounter >= params.getNT()) {
          ESTAGE = spgxyz::increment<eStages>(ESTAGE);
          clearScreen(g_hWnd, g_wbrush);
        }
        TSTAGE = spgxyz::increment<tStages>(TSTAGE);
        segmentTimer.start(); // for the next ITI
      } else
        segmentTimer.resume();
      break;
    }
    break;

  case eStages::ENDMSG:

    MessageBox(NULL, getwstr(params.getEndMsg()).c_str(),
               getwstr(messages["cc"]).c_str(),
               MB_SYSTEMMODAL | MB_OK | MB_SETFOREGROUND);
    ESTAGE = spgxyz::increment<eStages>(ESTAGE);
    break;
  case eStages::END:
    return false;
  }
  return true;
}

void clearScreen(HWND hw, HBRUSH hb) {
  RECT rect;
  GetClientRect(hw, &rect);
  FillRect(GetDC(hw), &rect, hb);
}

void showTrial(HWND hw) {
  HDC hdc = GetDC(g_hWnd);
  auto buff = SetBkMode(hdc, TRANSPARENT);
  DrawText(hdc, getwstr(messages["clkMsg"]).c_str(), -1, &g_MsgRect1,
           DT_CENTER || DT_TOP);
  SetBkMode(hdc, buff);
  ReleaseDC(g_hWnd, hdc);
  showButtons(buttons);
}

std::pair<double, double> getFeedBack(std::string bl, ExptParams const &ep) {
  auto [wa, wp, la, lp] = ep.getParams(bl);
  auto gain = 0.0;
  if (g_urd(g_rng) < wp)
    gain += wa;
  auto loss = 0.0;
  if (g_urd(g_rng) < lp)
    loss += la;
  return std::make_pair(gain, loss);
}

std::string fmt(double a, ExptParams const &ep) {
  std::ostringstream marker;
  marker.precision(2);
  marker << std::fixed << a << "  " << ep.getCurrency();
  return marker.str();
}

void writeData(std::string f, std::vector<TData> const &data) {
  std::ofstream ouf{f, std::ios::app};
  if (!ouf)
    throw std::runtime_error{"cannot open file: " + f + "in " + __func__};
  for (auto &t : data) {
    ouf << t.ID << "\t" << t.TN << "\t" << t.deck << "\t" << t.win << "\t"
        << t.loss << "\t" << t.net << "\t" << t.balance << "\t" << t.dts;
  }
  ouf.flush();
}
