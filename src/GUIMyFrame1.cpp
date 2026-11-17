#include "GUIMyFrame1.h"
#include <vector>
#include <fstream>
#include "vecmat.h"

struct Point {
 float x, y, z;
 Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Color {
 int R, G, B;
 Color(int _R, int _G, int _B) : R(_R), G(_G), B(_B) {}
};

struct Segment {
 Point begin, end;
 Color color;
 Segment(Point _begin, Point _end, Color _color) : begin(_begin), end(_end), color(_color) {}
};

std::vector<Segment> data;
static wxBitmap g_bmp;
static wxString g_lastKey;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_button_load_geometry->SetLabel(_("Wczytaj Geometrie"));
 m_staticText25->SetLabel(_("Obrot X:"));
 m_staticText27->SetLabel(_("Obrot Y:"));
 m_staticText29->SetLabel(_("Obrot Z:"));

 WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
 WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
 WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

 WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
 WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
 WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

 WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
 WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
 WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);

 WxPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
WxPanel->Bind(wxEVT_PAINT, [this](wxPaintEvent&)
{
    wxPaintDC pdc(WxPanel);
    pdc.SetBackground(*wxWHITE_BRUSH);
    pdc.Clear();
    if (g_bmp.IsOk()) pdc.DrawBitmap(g_bmp, 0, 0);
});
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
 Repaint();
}

void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
 wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

 if (WxOpenFileDialog.ShowModal() == wxID_OK)
 {
  double x1, y1, z1, x2, y2, z2;
  int r, g, b;

  std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
  if (in.is_open())
  {
   data.clear();
   while (!in.eof())
   {
    in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b;
    data.push_back(Segment(Point(x1, y1, z1), Point(x2, y2, z2), Color(r, g, b)));
   }
   in.close();
  }
 }
}

void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));


Repaint();
}


    void GUIMyFrame1::Repaint()
{

    int w, h;
    WxPanel->GetClientSize(&w, &h);
    if (w <= 0 || h <= 0) return;


    wxBitmap bmp(w, h);
    wxMemoryDC dc(bmp);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    if (data.empty()) return;

    // parametry z suwaków
    const double deg2rad = 3.14159265358979323846 / 180.0;

    const double tx = (WxSB_TranslationX->GetValue() - 100) / 50.0;
    const double ty = (WxSB_TranslationY->GetValue() - 100) / 50.0;
    const double tz = (WxSB_TranslationZ->GetValue() - 100) / 50.0;

    const double ax = WxSB_RotateX->GetValue() * deg2rad;
    const double ay = WxSB_RotateY->GetValue() * deg2rad;
    const double az = WxSB_RotateZ->GetValue() * deg2rad;

    const double sx = WxSB_ScaleX->GetValue() / 100.0;
    const double sy = WxSB_ScaleY->GetValue() / 100.0;
    const double sz = WxSB_ScaleZ->GetValue() / 100.0;

    const double cxr = std::cos(ax), sxr = std::sin(ax);
    const double cyr = std::cos(ay), syr = std::sin(ay);
    const double czr = std::cos(az), szr = std::sin(az);

    const double f    = 3.0;   // odległość kamery od środka sceny
    const double nearP = 0.1;  // płaszczyzna odcinania

    const double cx = w / 2.0;
    const double cy = h / 2.0;
    const double s  = std::min(w, h) / 3.0;   // skala na ekran

    // przekształcenie punktu
    auto transform = [&](const Point& p, double out[3])
    {
        double x = p.x * sx, y = p.y * sy, z = p.z * sz;

        // obrót wokół X
        double y1 = y * cxr - z * sxr;
        double z1 = y * sxr + z * cxr;
        y = y1; z = z1;

        // obrót wokół Y
        double x2 =  x * cyr + z * syr;
        double z2 = -x * syr + z * cyr;
        x = x2; z = z2;

        // obrót wokół Z
        double x3 = x * czr - y * szr;
        double y3 = x * szr + y * czr;
        x = x3; y = y3;

        out[0] = x + tx;
        out[1] = y + ty;
        out[2] = z + tz;
    };

    for (const Segment& seg : data)
    {
        double a[3], b[3];
        transform(seg.begin, a);
        transform(seg.end, b);

        double wa = a[2] + f;
        double wb = b[2] + f;

        // oba końce za kamerą 
        if (wa < nearP && wb < nearP) continue;

        // obcięcie do płaszczyzny near
        if (wa < nearP)
        {
            double t = (nearP - wa) / (wb - wa);
            for (int i = 0; i < 3; i++) a[i] += t * (b[i] - a[i]);
            wa = a[2] + f;
        }
        else if (wb < nearP)
        {
            double t = (nearP - wb) / (wa - wb);
            for (int i = 0; i < 3; i++) b[i] += t * (a[i] - b[i]);
            wb = b[2] + f;
        }

        // rzut perspektywy
        double x1 = cx + s * (a[0] * f / wa);
        double y1 = cy - s * (a[1] * f / wa);
        double x2 = cx + s * (b[0] * f / wb);
        double y2 = cy - s * (b[1] * f / wb);

        dc.SetPen(wxPen(wxColour(seg.color.R, seg.color.G, seg.color.B)));
        dc.DrawLine(wxRound(x1), wxRound(y1), wxRound(x2), wxRound(y2));
    }

    dc.SelectObject(wxNullBitmap);
    g_bmp = bmp;
    WxPanel->Refresh(false);
}