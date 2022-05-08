#include <Windows.h>
#include <math.h>
struct Point {
    int x, y;

    Point() {
        x = 0; y = 0;
    }

    Point(int x, int y) {
        this->x = x; this->y = y;
    }
};

void swap(Point& a, Point& b)
{
    Point tmp = a;
    a = b;
    b = tmp;
}

void swap(int& x, int& y)
{
    int tmp = x;
    x = y;
    y = tmp;
}

int Round(double x)
{
    return (int)(x + 0.5);
}

void DrawPoints(HDC hdc, Point center, int a, int b)
{
    SetPixel(hdc, center.x + a, center.y + b, RGB(255, 0, 0));
    SetPixel(hdc, center.x - a, center.y + b, RGB(255, 0, 0));
    SetPixel(hdc, center.x - a, center.y - b, RGB(255, 0, 0));
    SetPixel(hdc, center.x + a, center.y - b, RGB(255, 0, 0));
    SetPixel(hdc, center.x + b, center.y + a, RGB(255, 0, 0));
    SetPixel(hdc, center.x - b, center.y + a, RGB(255, 0, 0));
    SetPixel(hdc, center.x - b, center.y - a, RGB(255, 0, 0));
    SetPixel(hdc, center.x + b, center.y - a, RGB(255, 0, 0));
}

void CircleBresenham(HDC hdc, Point center, int R)
{
    int x = 0, y = R;
    int d = 1 - R;
    int c1 = 3, c2 = 5 - 2 * R;
    DrawPoints(hdc, center, x, y);
    while (x < y)
    {
        if (d < 0)
        {
            d += c1;
            c2 += 2;
        }
        else
        {
            d += c2;
            c2 += 4;
            y--;
        }
        c1 += 2;
        x++;
        DrawPoints(hdc, center, x, y);
    }
}

void DrawPointCircleClipping(HDC hdc, Point p, Point cCenter, int r) {
    if ((p.x - cCenter.x) * (p.x - cCenter.x) + (p.y - cCenter.y) * (p.y - cCenter.y) > r * r) {
        SetPixel(hdc, p.x, p.y, RGB(0, 0, 255));
    }
    else {
        SetPixel(hdc, p.x, p.y, RGB(255, 0, 0));
    }
}

void DrawCurve(HDC hdc, Point b[], Point cCenter, int r) {
    // TODO
    for (int i = 0; i < 4; i++)
    {
        DrawPointCircleClipping(hdc, b[i], cCenter, r);
    }
}

static int state = 0;
static Point center, b1[4], b2[4], b3[4], b4[4];
static int r;

LRESULT WINAPI MyWndProc(HWND hwnd, UINT mcode, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    Point mouse = Point(LOWORD(lp), HIWORD(lp));;

    switch (mcode)
    {
    case WM_LBUTTONDOWN:
        hdc = GetDC(hwnd);

        switch (state) {
        case 0:
            center = mouse;
            break;
        case 1:
            r = (int)sqrt(((mouse.x - center.x) * (mouse.x - center.x)) + ((mouse.y - center.y) * (mouse.y - center.y)));
            CircleBresenham(hdc, center, r);
            break;
        default:
            if (state < 6) {
                b1[state - 2] = mouse;

                if (state == 5)
                    DrawCurve(hdc, b1, center, r);
            }
            else if (state < 10) {
                b2[state - 6] = mouse;

                if (state == 9)
                    DrawCurve(hdc, b2, center, r);
            }
            else if (state < 14) {
                b3[state - 10] = mouse;

                if (state == 13)
                    DrawCurve(hdc, b3, center, r);
            }
            else {
                b4[state - 14] = mouse;

                if (state == 17)
                    DrawCurve(hdc, b4, center, r);
            }
            break;
        }
        state++;
        if (state == 18)
            state = 0;

        ReleaseDC(hwnd, hdc);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, mcode, wp, lp);
    }
    return 0;
}
int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmd, int nsh)
{
    WNDCLASS wc;
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = hinst;
    wc.lpfnWndProc = MyWndProc;
    wc.lpszClassName = L"MyClass";
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(L"MyClass", L"My First Window", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hinst, 0);
    ShowWindow(hwnd, nsh);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}