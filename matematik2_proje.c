/*******************************************************************************
 *  MATHEMATICS-II PROJE ODEVI
 *  Belirli Integral ile Alan Hesaplama - 10 Soru
 *
 *  Derleme  : gcc matematik2_proje.c -o matematik2_proje -lm
 *  Calistirma: ./matematik2_proje
 *
 *  Ozellikler:
 *    - Simpson 1/3 sayisal integral yontemi
 *    - Her soru icin terminal tabanli ASCII grafik cizimi
 *    - Temiz kod (clean code) prensipleri
 *    - Detayli yorum satirlari (matematiksel isleme karsilik gelen)
 *    - Konsol menusu (switch-case)
 ******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 *  BOLUM 1 : SABITLER VE TURLER
 * ======================================================================== */

#define PI            3.14159265358979323846
#define SIMPSON_ADIM  10000          /* Simpson bolme sayisi (cift olmali) */
#define GRAF_GENISLIK 72             /* ASCII grafik genisligi (sutun)   */
#define GRAF_YUKSEK   24             /* ASCII grafik yuksekligi (satir) */

/* Fonksiyon isaretcisi: double alan, double donen tek parametreli fonksiyon */
typedef double (*Fonksiyon)(double);

/* Bir fonksiyonun grafikte nasil cizilecegini tutan yapi */
typedef struct {
    Fonksiyon  ptr;       /* fonksiyon isaretcisi          */
    char       karakter;  /* cizimde kullanilacak karakter  */
} FonksiyonGrafik;

/* Hesaplanan alan bolgesinin sinirlarini tutan yapi */
typedef struct {
    double  x_alt;        /* bolgenin sol x siniri          */
    double  x_ust;        /* bolgenin sag x siniri          */
    Fonksiyon ust_sinir;   /* ust sinir fonksiyonu           */
    Fonksiyon alt_sinir;   /* alt sinir fonksiyonu           */
} AlanBolgesi;

/* Grafik cizim parametrelerini tek bir yapi olarak birlestiren ana yapi */
typedef struct {
    const char *baslik;                            /* grafik basligi            */

    double      x_min, x_max;                     /* x ekseni gorunum araligi  */
    double      y_min, y_max;                     /* y ekseni gorunum araligi  */

    int         fonk_sayisi;                      /* cizilecek fonksiyon sayisi */
    FonksiyonGrafik fonksiyonlar[3];              /* fonksiyon listesi          */

    int         alan_sayisi;                      /* alan bolgesi sayisi        */
    AlanBolgesi alanlar[2];                       /* alan bolgeleri             */

    int         nokta_sayisi;                     /* isaretlenecek ozel nokta   */
    double      nokta_x[6];                       /* nokta x koordinatlari      */
    double      nokta_y[6];                       /* nokta y koordinatlari      */
    char        nokta_karakter[6];                /* nokta cizim karakteri       */
} GrafikAyarlari;

/* ========================================================================
 *  BOLUM 2 : GENEL SAYISAL INTEGRAL FONKSIYONLARI (Simpson 1/3 Yontemi)
 *
 *  Simpson 1/3 Formul:
 *    integral ~ (h/3) * [f(a) + f(b) + 4*sum(tek endeksli) + 2*sum(cift)]
 *    burada h = (b - a) / n
 * ======================================================================== */

/**
 * Tek bir fonksiyonun [a, b] araliginda sayisal integralini hesaplar.
 * Simpson 1/3 yontemini kullanir.
 *
 * @param func  : integrali alinacak fonksiyon
 * @param a     : alt sinir
 * @param b     : ust sinir
 * @param n     : bolme sayisi (cift sayi olmali)
 * @return      : yaklasik integral degeri
 */
double simpson_integral(Fonksiyon func, double a, double b, int n)
{
    double h = (b - a) / n;       /* adim genisligi (delta x)             */
    double toplam = func(a) + func(b); /* ilk + son terim                 */

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;    /* i. alt-bolme noktasi                  */

        /* Simpson katsayisi: tek endeksli -> 4, cift endeksli -> 2       */
        if (i % 2 == 1)
            toplam += 4.0 * func(xi);
        else
            toplam += 2.0 * func(xi);
    }

    return (h / 3.0) * toplam;   /* (h/3) ile carpim -> integral yaklasigi */
}

/**
 * Bir fonksiyonun MUTLAK DEGERLI integralini hesaplar.
 * x ekseni ile arasindaki alani bulmak icin kullanilir
 * (fonksiyon x ekseninin altinda kaldiğinda da pozitif alan verir).
 *
 * Matematiksel karsilik: int_a^b |f(x)| dx
 *
 * @param func  : fonksiyon
 * @param a     : alt sinir
 * @param b     : ust sinir
 * @param n     : bolme sayisi
 * @return      : |f(x)| fonksiyonunun integrali
 */
double simpson_mutlak_degerli(Fonksiyon func, double a, double b, int n)
{
    double h = (b - a) / n;
    double toplam = fabs(func(a)) + fabs(func(b));

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        /* Her noktada mutlak deger al -> negatif degerler pozitif olur    */
        if (i % 2 == 1)
            toplam += 4.0 * fabs(func(xi));
        else
            toplam += 2.0 * fabs(func(xi));
    }

    return (h / 3.0) * toplam;
}

/**
 * Iki fonksiyon arasindaki alani hesaplar.
 * Ust fonksiyon - alt fonksiyon farkinin integralini alir.
 *
 * Matematiksel karsilik: int_a^b [f_ust(x) - f_alt(x)] dx
 *
 * @param ust   : ust sinir fonksiyonu
 * @param alt   : alt sinir fonksiyonu
 * @param a     : alt sinir
 * @param b     : ust sinir
 * @param n     : bolme sayisi
 * @return      : iki fonksiyon arasindaki alan
 */
double simpson_fark(Fonksiyon ust, Fonksiyon alt, double a, double b, int n)
{
    double h = (b - a) / n;
    double toplam = (ust(a) - alt(a)) + (ust(b) - alt(b));

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        double fark = ust(xi) - alt(xi); /* nokta farki                    */
        if (i % 2 == 1)
            toplam += 4.0 * fark;
        else
            toplam += 2.0 * fark;
    }

    return (h / 3.0) * toplam;
}

/* ========================================================================
 *  BOLUM 3 : SORUYA OZEL FONKSIYON TANIMLARI
 *
 *  Her soruda gecen matematiksel fonksiyonlar burada tanimlanir.
 *  Isimlendirme kurali: "soruX_" on eki + fonksiyonun anlami
 * ======================================================================== */

/* --- Soru 1: f(x) = x^2 - 4 ---------------------------------------- */
double soru1_fonksiyon(double x) { return x * x - 4.0; }

/* --- Soru 2: f(x) = sin(x) ----------------------------------------- */
double soru2_sin(double x) { return sin(x); }

/* --- Soru 3: y = sin(x) ve y = cos(x) ------------------------------ */
double soru3_sin(double x) { return sin(x); }
double soru3_cos(double x) { return cos(x); }

/* --- Soru 4: y = sqrt(x) ve y = 3 (sabit dogru) ------------------- */
double soru4_kok(double x)  { return sqrt(x); }
double sabit_uc(double x)   { (void)x; return 3.0; }   /* y = 3 sabiti */

/* --- Soru 5: y = x^2 - 2  ve  y = 3x - x^2 ----------------------- */
double soru5_parabol1(double x) { return x * x - 2.0; }
double soru5_parabol2(double x) { return 3.0 * x - x * x; }

/* --- Soru 6: y = x^2 (parabol) ve y = 4x - 4 (teget dogrusu) ----- */
double soru6_parabol(double x) { return x * x; }
double soru6_teget(double x)   { return 4.0 * x - 4.0; }

/* --- Soru 7: y = x(x^2 - 1) = x^3 - x ---------------------------- */
double soru7_fonksiyon(double x) { return x * x * x - x; }

/* --- Soru 8: f(x) = x * sin(x) ------------------------------------ */
double soru8_fonksiyon(double x) { return x * sin(x); }

/* --- Soru 9: f(x) = x^4 + 6x^3 + 9x^2 = x^2(x+3)^2 -------------- */
double soru9_fonksiyon(double x) { return x*x*x*x + 6.0*x*x*x + 9.0*x*x; }

/* --- Soru 10: y = 7e^x ve y = 3 - 2e^x --------------------------- */
double soru10_ust(double x) { return 7.0 * exp(x); }
double soru10_alt(double x) { return 3.0 - 2.0 * exp(x); }

/* --- Genel yardimci: sifir fonksiyonu (x ekseni) -------------------- */
double sifir_fonk(double x) { (void)x; return 0.0; }

/* ========================================================================
 *  BOLUM 4 : ASCII GRAFIK CIIZIM MODULU
 *
 *  Terminalde 2 boyutlu karakter tabanli grafik cizer.
 *  '*' = fonksiyon egrisi
 *  '#' = hesaplanan alan bolgesi
 *  'O' = ozel noktalar (kesim, teget vb.)
 *  '-' ve '|' = eksenler
 *  '+' = orijin
 * ======================================================================== */

/**
 * y degerini grafik satir indeksine donusturur.
 * y_max -> satir 0 (en ust), y_min -> satir (YUKSEK-1) (en alt).
 */
static int y_to_satir(double y, double y_min, double y_max)
{
    if (y_max == y_min) return GRAF_YUKSEK / 2;
    int satir = (int)round((y_max - y) / (y_max - y_min) * (GRAF_YUKSEK - 1));
    if (satir < 0) return 0;
    if (satir >= GRAF_YUKSEK) return GRAF_YUKSEK - 1;
    return satir;
}

/**
 * x degerini grafik sutun indeksine donusturur.
 * x_min -> sutun 0 (en sol), x_max -> sutun (GENIS-1) (en sag).
 */
static int x_to_sutun(double x, double x_min, double x_max)
{
    if (x_max == x_min) return GRAF_GENISLIK / 2;
    int sutun = (int)round((x - x_min) / (x_max - x_min) * (GRAF_GENISLIK - 1));
    if (sutun < 0) return 0;
    if (sutun >= GRAF_GENISLIK) return GRAF_GENISLIK - 1;
    return sutun;
}

/**
 * Genel ASCII grafik cizim fonksiyonu.
 *
 * Verilen parametrelere gore:
 *   1. Bos bir tuval olusturur
 *   2. Alan bolgelerini '#' ile doldurur
 *   3. x ve y eksenlerini cizer
 *   4. Fonksiyon egrilerini '*' ile cizer
 *   5. Ozel noktalari 'O' ile isaretler
 *   6. Eksen etiketlerini ve aciklamayi yazdirir
 */
void ascii_grafik_ciz(const GrafikAyarlari *g)
{
    /* --- 4.1 Tuvali bosluklarla doldur --- */
    char tuval[GRAF_YUKSEK][GRAF_GENISLIK];
    for (int r = 0; r < GRAF_YUKSEK; r++)
        for (int c = 0; c < GRAF_GENISLIK; c++)
            tuval[r][c] = ' ';

    /* --- 4.2 Alan bolgelerini '#' karakteriyle doldur --- */
    /* Her alan bolgesi icin: x araligindaki her sutunda ust ve alt sinir
       arasindaki tum satirlari '#' ile isaretle                                 */
    for (int a = 0; a < g->alan_sayisi; a++) {
        for (int c = 0; c < GRAF_GENISLIK; c++) {
            /* sutun -> x koordinati */
            double x = g->x_min + (double)c / (GRAF_GENISLIK - 1)
                       * (g->x_max - g->x_min);

            /* x, alan bolgesinin araliginda mi? */
            if (x >= g->alanlar[a].x_alt && x <= g->alanlar[a].x_ust) {
                /* ust ve alt sinir degerlerini hesapla */
                double y_ust_deger = g->alanlar[a].ust_sinir(x);
                double y_alt_deger = g->alanlar[a].alt_sinir(x);

                /* y degerlerini satir indeksine cevir */
                int sat_ust = y_to_satir(y_ust_deger, g->y_min, g->y_max);
                int sat_alt = y_to_satir(y_alt_deger, g->y_min, g->y_max);

                /* ust satir ile alt satir arasini '#' ile doldur               */
                int baslangic = sat_ust < sat_alt ? sat_ust : sat_alt;
                int bitis     = sat_ust < sat_alt ? sat_alt : sat_ust;
                for (int r = baslangic; r <= bitis; r++)
                    tuval[r][c] = '#';
            }
        }
    }

    /* --- 4.3 x ve y eksenlerini cizer --- */
    int satir_sifir = y_to_satir(0.0, g->y_min, g->y_max);
    int sutun_sifir = x_to_sutun(0.0, g->x_min, g->x_max);

    /* yatay eksen (y = 0) -> tum sutunlarda '-' */
    if (satir_sifir >= 0 && satir_sifir < GRAF_YUKSEK) {
        for (int c = 0; c < GRAF_GENISLIK; c++)
            if (tuval[satir_sifir][c] == ' ')
                tuval[satir_sifir][c] = '-';
    }

    /* dikey eksen (x = 0) -> tum satirlarda '|' */
    if (sutun_sifir >= 0 && sutun_sifir < GRAF_GENISLIK) {
        for (int r = 0; r < GRAF_YUKSEK; r++)
            if (tuval[r][sutun_sifir] == ' ')
                tuval[r][sutun_sifir] = '|';
    }

    /* orijin '+' isareti */
    if (satir_sifir >= 0 && satir_sifir < GRAF_YUKSEK &&
        sutun_sifir >= 0 && sutun_sifir < GRAF_GENISLIK)
        tuval[satir_sifir][sutun_sifir] = '+';

    /* --- 4.4 Fonksiyon egrilerini '*' ile cizer --- */
    for (int f = 0; f < g->fonk_sayisi; f++) {
        for (int c = 0; c < GRAF_GENISLIK; c++) {
            double x = g->x_min + (double)c / (GRAF_GENISLIK - 1)
                       * (g->x_max - g->x_min);
            double y = g->fonksiyonlar[f].ptr(x);
            int r = y_to_satir(y, g->y_min, g->y_max);
            /* fonksiyon degeri gorunum alanindaysa, '*' karakterini koy */
            if (r >= 0 && r < GRAF_YUKSEK)
                tuval[r][c] = g->fonksiyonlar[f].karakter;
        }
    }

    /* --- 4.5 Ozel noktalari 'O' ile isaretler (en ustte, her zaman gorunur) --- */
    for (int n = 0; n < g->nokta_sayisi; n++) {
        int c = x_to_sutun(g->nokta_x[n], g->x_min, g->x_max);
        int r = y_to_satir(g->nokta_y[n], g->y_min, g->y_max);
        if (r >= 0 && r < GRAF_YUKSEK && c >= 0 && c < GRAF_GENISLIK)
            tuval[r][c] = g->nokta_karakter[n];
    }

    /* --- 4.6 Cikti: grafik basligi --- */
    printf("\n");
    printf("        %s\n", g->baslik);
    printf("      +");
    for (int c = 0; c < GRAF_GENISLIK; c++) printf("-");
    printf("+\n");

    /* --- 4.7 Cikti: tuvali satir satir yazdir (her satira y etiketi ekle) --- */
    for (int r = 0; r < GRAF_YUKSEK; r++) {
        /* Bu satira karsilik gelen y degerini hesapla ve etiket olarak yaz */
        double y_deger = g->y_max - (double)r / (GRAF_YUKSEK - 1)
                         * (g->y_max - g->y_min);
        printf("%6.1f |", y_deger);

        for (int c = 0; c < GRAF_GENISLIK; c++)
            putchar(tuval[r][c]);

        printf("|\n");
    }

    /* --- 4.8 Cikti: alt cerceve ve x ekseni etiketleri --- */
    printf("      +");
    for (int c = 0; c < GRAF_GENISLIK; c++) printf("-");
    printf("+\n");
    printf("      ");
    printf("%7.1f", g->x_min);

    /* x ekseni orta degerlerini yaz */
    int adim = GRAF_GENISLIK / 4;
    for (int c = adim; c < GRAF_GENISLIK - adim / 2; c += adim) {
        double x_deger = g->x_min + (double)c / (GRAF_GENISLIK - 1)
                         * (g->x_max - g->x_min);
        /* cursor'u dogru konuma tasimak icin bosluk ekle */
        int mevcut_pos = 7 + c;
        int hedef_pos  = 7 + c;
        while (mevcut_pos < hedef_pos) { putchar(' '); mevcut_pos++; }
        printf("%5.1f", x_deger);
        mevcut_pos += 5;
    }

    printf("%7.1f\n", g->x_max);

    /* --- 4.9 Aciklama (legend) --- */
    printf("\n");
    printf("      * = Fonksiyon egrisi    # = Hesaplanan alan bolgesi");
    printf("    O = Kesim/ozel nokta\n\n");
}

/* ========================================================================
 *  BOLUM 5 : SORU COZUM FONKSIYONLARI
 *
 *  Her soru icin ayri bir fonksiyon:
 *    1. Sorunun matematiksel ozetini yazdirir
 *    2. Sayisal integral ile alani hesaplar
 *    3. Analitik sonucu karsilastirmali yazdirir
 *    4. ASCII grafik cizer
 * ======================================================================== */

/* -------------------------------------------------------------------- */
/*  SORU 1: f(x) = x^2 - 4, Aralik: [-2, 3]                         */
/* -------------------------------------------------------------------- */
void soru_1_coz(void)
{
    const char *baslik = "SORU 1: f(x) = x^2 - 4 ile x Ekseni Arasindaki Alan [-2, 3]";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    /* Matematiksel aciklama */
    printf("  MATEMATIKSEL COZUM:\n");
    printf("  f(x) = x^2 - 4 fonksiyonunun x eksenini kestigi noktalar:\n");
    printf("    x^2 - 4 = 0  =>  x = -2, x = +2\n\n");
    printf("  [-2, 2] araliginda f(x) <= 0 (x ekseninin ALTINDA)\n");
    printf("  [ 2, 3] araliginda f(x) >= 0 (x ekseninin USTUNDE)\n\n");
    printf("  Alan = |int_{-2}^{2} (x^2-4) dx| + int_{2}^{3} (x^2-4) dx\n");
    printf("       = 32/3 + 7/3 = 39/3 = 13\n\n");

    /* Sayisal hesaplama */
    double alan_bolge1 = simpson_integral(soru1_fonksiyon, -2.0, 2.0, SIMPSON_ADIM);
    double alan_bolge2 = simpson_integral(soru1_fonksiyon,  2.0, 3.0, SIMPSON_ADIM);
    double toplam_alan = fabs(alan_bolge1) + alan_bolge2;

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{-2}^{2} (x^2-4) dx = %12.10f  (negatif -> mutlak al)\n", alan_bolge1);
    printf("    |int_{-2}^{2} (x^2-4) dx| = %12.10f\n", fabs(alan_bolge1));
    printf("    int_{2}^{3} (x^2-4) dx   = %12.10f\n", alan_bolge2);
    printf("    ------------------------------\n");
    printf("    TOPLAM ALAN             = %12.10f\n", toplam_alan);
    printf("    Analitik Sonuc          = %12.10f  (13)\n\n", 13.0);

    /* ASCII Grafik */
    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -2.5, .x_max = 3.5,
        .y_min = -6.0, .y_max = 6.0,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru1_fonksiyon, '*' }
        },
        .alan_sayisi = 2,
        .alanlar = {
            /* Bolge 1: [-2,2] arasinda, ust=0 (x ekseni), alt=f(x) */
            { -2.0, 2.0, sifir_fonk, soru1_fonksiyon },
            /* Bolge 2: [2,3] arasinda, ust=f(x), alt=0 (x ekseni) */
            {  2.0, 3.0, soru1_fonksiyon, sifir_fonk }
        },
        .nokta_sayisi = 2,
        .nokta_x = { -2.0, 2.0 },
        .nokta_y = { 0.0, 0.0 },
        .nokta_karakter = { 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 2: f(x) = sin(x), Aralik: [-pi, pi]                          */
/* -------------------------------------------------------------------- */
void soru_2_coz(void)
{
    const char *baslik = "SORU 2: f(x) = sin(x) ile x Ekseni Arasindaki Alan [-pi, pi]";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  f(x) = sin(x), [-pi, pi] araligi\n");
    printf("  sin(x) = 0 kokleri: x = -pi, x = 0, x = pi\n\n");
    printf("  sin(x) [-pi,0] araliginda NEGATIF, [0,pi] araliginda POZITIF.\n");
    printf("  Alan hesaplamada MUTLAK DEGER alinmalidir.\n\n");
    printf("  Alan = int_{-pi}^{pi} |sin(x)| dx = 2 * int_{0}^{pi} sin(x) dx = 4\n\n");

    /* Sayisal hesaplama */
    double alan_negatif_bolge = fabs(simpson_integral(soru2_sin, -PI, 0.0, SIMPSON_ADIM));
    double alan_pozitif_bolge = simpson_integral(soru2_sin, 0.0,  PI, SIMPSON_ADIM);
    double toplam_alan = alan_negatif_bolge + alan_pozitif_bolge;

    /* Alternatif: |sin(x)| integrali */
    double alan_mutlak = simpson_mutlak_degerli(soru2_sin, -PI, PI, SIMPSON_ADIM);

    printf("  SAYISAL SONUCLAR:\n");
    printf("    |int_{-pi}^{0} sin(x) dx| = %12.10f\n", alan_negatif_bolge);
    printf("     int_{0}^{pi} sin(x) dx  = %12.10f\n", alan_pozitif_bolge);
    printf("    Toplam (bolge bazli)      = %12.10f\n", toplam_alan);
    printf("    int |sin(x)| dx           = %12.10f\n", alan_mutlak);
    printf("    Analitik Sonuc            = %12.10f  (4)\n\n", 4.0);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -3.5, .x_max = 3.5,
        .y_min = -1.5, .y_max = 1.5,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru2_sin, '*' }
        },
        .alan_sayisi = 2,
        .alanlar = {
            /* [-pi, 0]: sin(x) <= 0, ust = 0, alt = sin(x) */
            { -PI, 0.0, sifir_fonk, soru2_sin },
            /* [0, pi]: sin(x) >= 0, ust = sin(x), alt = 0 */
            { 0.0, PI, soru2_sin, sifir_fonk }
        },
        .nokta_sayisi = 3,
        .nokta_x = { -PI, 0.0, PI },
        .nokta_y = { 0.0, 0.0, 0.0 },
        .nokta_karakter = { 'O', 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 3: y = sin(x), y = cos(x), Aralik: [0, pi/2]                 */
/* -------------------------------------------------------------------- */
void soru_3_coz(void)
{
    const char *baslik = "SORU 3: y=sin(x), y=cos(x) ile x Ekseni Arasi [0, pi/2]";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    double pi_dort = PI / 4.0;
    double pi_iki  = PI / 2.0;

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  Iki egrinin kesiim noktasi:\n");
    printf("    sin(x) = cos(x)  =>  x = pi/4 = %.6f\n\n", pi_dort);
    printf("  [0, pi/4]    : cos(x) >= sin(x)  (cos ustte)\n");
    printf("  [pi/4, pi/2] : sin(x) >= cos(x)  (sin ustte)\n\n");
    printf("  Alan = int_{0}^{pi/4}(cos-sin)dx + int_{pi/4}^{pi/2}(sin-cos)dx\n");
    printf("       = 2(sqrt(2) - 1) = %.10f\n\n", 2.0 * (sqrt(2.0) - 1.0));

    double alan1 = simpson_fark(soru3_cos, soru3_sin, 0.0, pi_dort, SIMPSON_ADIM);
    double alan2 = simpson_fark(soru3_sin, soru3_cos, pi_dort, pi_iki, SIMPSON_ADIM);
    double toplam = alan1 + alan2;

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{0}^{pi/4} (cos x - sin x) dx   = %12.10f\n", alan1);
    printf("    int_{pi/4}^{pi/2} (sin x - cos x) dx = %12.10f\n", alan2);
    printf("    Toplam Alan                        = %12.10f\n", toplam);
    printf("    Analitik Sonuc                     = %12.10f\n\n",
           2.0 * (sqrt(2.0) - 1.0));

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -0.2, .x_max = 1.8,
        .y_min = -0.15, .y_max = 1.15,
        .fonk_sayisi = 2,
        .fonksiyonlar = {
            { soru3_sin, '*' },
            { soru3_cos, '+' }
        },
        .alan_sayisi = 2,
        .alanlar = {
            { 0.0, pi_dort, soru3_cos, soru3_sin },
            { pi_dort, pi_iki, soru3_sin, soru3_cos }
        },
        .nokta_sayisi = 1,
        .nokta_x = { pi_dort },
        .nokta_y = { sqrt(2.0) / 2.0 },
        .nokta_karakter = { 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 4: y = sqrt(x), y = 3, y ekseni (x = 0)                      */
/* -------------------------------------------------------------------- */
void soru_4_coz(void)
{
    const char *baslik = "SORU 4: y=sqrt(x), y=3 ve y Ekseni ile Sinirli Alan";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  sqrt(x) = 3  =>  x = 9 (kesim noktasi)\n\n");
    printf("  Bolge sinirlari: x = 0 (y ekseni), x = 9\n");
    printf("  Ust sinir: y = 3 (sabit dogru)\n");
    printf("  Alt sinir: y = sqrt(x)\n\n");
    printf("  Alan = int_{0}^{9} (3 - sqrt(x)) dx\n");
    printf("       = [3x - (2/3)x^(3/2)]_{0}^{9} = 27 - 18 = 9\n\n");

    double alan = simpson_fark(sabit_uc, soru4_kok, 0.0, 9.0, SIMPSON_ADIM);

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{0}^{9} (3 - sqrt(x)) dx = %12.10f\n", alan);
    printf("    Analitik Sonuc              = %12.10f  (9)\n\n", 9.0);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -0.5, .x_max = 10.5,
        .y_min = -0.5, .y_max = 4.5,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru4_kok, '*' }
        },
        .alan_sayisi = 1,
        .alanlar = {
            { 0.0, 9.0, sabit_uc, soru4_kok }
        },
        .nokta_sayisi = 1,
        .nokta_x = { 9.0 },
        .nokta_y = { 3.0 },
        .nokta_karakter = { 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 5: y = x^2 - 2  ve  y = 3x - x^2                             */
/* -------------------------------------------------------------------- */
void soru_5_coz(void)
{
    const char *baslik = "SORU 5: y=x^2-2 ve y=3x-x^2 Parabolleri Arasindaki Alan";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  Kesim: x^2-2 = 3x-x^2  =>  2x^2-3x-2 = 0\n");
    printf("         (2x+1)(x-2) = 0  =>  x = -0.5, x = 2\n\n");
    printf("  [-0.5, 2] araliginda 3x-x^2 >= x^2-2 (3x-x^2 ustte)\n\n");
    printf("  Alan = int_{-0.5}^{2} [(3x-x^2) - (x^2-2)] dx\n");
    printf("       = int_{-0.5}^{2} (3x - 2x^2 + 2) dx = 125/24\n\n");

    double alan = simpson_fark(soru5_parabol2, soru5_parabol1, -0.5, 2.0, SIMPSON_ADIM);

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{-0.5}^{2} [(3x-x^2)-(x^2-2)] dx = %12.10f\n", alan);
    printf("    Analitik Sonuc                         = %12.10f\n\n", 125.0/24.0);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -1.5, .x_max = 3.5,
        .y_min = -3.0, .y_max = 3.0,
        .fonk_sayisi = 2,
        .fonksiyonlar = {
            { soru5_parabol1, '*' },
            { soru5_parabol2, '+' }
        },
        .alan_sayisi = 1,
        .alanlar = {
            { -0.5, 2.0, soru5_parabol2, soru5_parabol1 }
        },
        .nokta_sayisi = 2,
        .nokta_x = { -0.5, 2.0 },
        .nokta_y = { (-0.5)*(-0.5)-2.0, 2.0*2.0-2.0 },
        .nokta_karakter = { 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 6: y = x^2 Parabolu, Teget y = 4x - 4, x ekseni               */
/* -------------------------------------------------------------------- */
void soru_6_coz(void)
{
    const char *baslik = "SORU 6: y=x^2 Parabolu, Teget y=4x-4 ve x Ekseni Arasi";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  Parabol: y = x^2\n");
    printf("  Turev:   dy/dx = 2x  =>  egim(2) = 4\n");
    printf("  Teget noktasi: (2, 4)\n\n");
    printf("  Teget dogrusu:  y - 4 = 4(x - 2)  =>  y = 4x - 4\n");
    printf("  Tegetin x eksenini kesimi: 4x - 4 = 0  =>  x = 1\n\n");
    printf("  Bolge 1: [0, 1] arasinda, parabol USTTE, x ekseni ALTTA\n");
    printf("  Bolge 2: [1, 2] arasinda, parabol USTTE, teget ALTTA\n\n");
    printf("  Alan = int_{0}^{1} x^2 dx + int_{1}^{2} [x^2 - (4x-4)] dx\n");
    printf("       = 1/3 + 1/3 = 2/3\n\n");

    double alan1 = simpson_fark(soru6_parabol, sifir_fonk, 0.0, 1.0, SIMPSON_ADIM);
    double alan2 = simpson_fark(soru6_parabol, soru6_teget, 1.0, 2.0, SIMPSON_ADIM);
    double toplam = alan1 + alan2;

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{0}^{1} [x^2 - 0] dx          = %12.10f\n", alan1);
    printf("    int_{1}^{2} [x^2 - (4x-4)] dx     = %12.10f\n", alan2);
    printf("    Toplam Alan                      = %12.10f\n", toplam);
    printf("    Analitik Sonuc                   = %12.10f\n\n", 2.0/3.0);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -0.3, .x_max = 2.5,
        .y_min = -1.0, .y_max = 5.0,
        .fonk_sayisi = 2,
        .fonksiyonlar = {
            { soru6_parabol, '*' },
            { soru6_teget, '+' }
        },
        .alan_sayisi = 2,
        .alanlar = {
            { 0.0, 1.0, soru6_parabol, sifir_fonk },
            { 1.0, 2.0, soru6_parabol, soru6_teget }
        },
        .nokta_sayisi = 2,
        .nokta_x = { 2.0, 1.0 },
        .nokta_y = { 4.0, 0.0 },
        .nokta_karakter = { 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 7: y = x(x^2 - 1) = x^3 - x                                   */
/* -------------------------------------------------------------------- */
void soru_7_coz(void)
{
    const char *baslik = "SORU 7: y=x(x^2-1) ve x Ekseni Kapali Bolgelerin Alani";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  y = x^3 - x = x(x-1)(x+1)\n");
    printf("  Kokler: x = -1, x = 0, x = 1\n\n");
    printf("  [-1, 0] : f(x) >= 0 (ust bolge)\n");
    printf("  [ 0, 1] : f(x) <= 0 (alt bolge)\n\n");
    printf("  Alan = int_{-1}^{0}(x^3-x)dx + int_{0}^{1}(x-x^3)dx = 1/4 + 1/4 = 1/2\n\n");

    double alan1 = simpson_integral(soru7_fonksiyon, -1.0, 0.0, SIMPSON_ADIM);
    double alan2 = fabs(simpson_integral(soru7_fonksiyon, 0.0, 1.0, SIMPSON_ADIM));
    double toplam = alan1 + alan2;

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{-1}^{0} (x^3-x) dx  = %12.10f  (pozitif)\n", alan1);
    printf("    |int_{0}^{1} (x^3-x) dx| = %12.10f  (negatif -> mutlak)\n", alan2);
    printf("    Toplam Alan             = %12.10f\n", toplam);
    printf("    Analitik Sonuc          = %12.10f\n\n", 0.5);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -1.5, .x_max = 1.5,
        .y_min = -0.8, .y_max = 0.8,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru7_fonksiyon, '*' }
        },
        .alan_sayisi = 2,
        .alanlar = {
            { -1.0, 0.0, soru7_fonksiyon, sifir_fonk },
            { 0.0, 1.0, sifir_fonk, soru7_fonksiyon }
        },
        .nokta_sayisi = 3,
        .nokta_x = { -1.0, 0.0, 1.0 },
        .nokta_y = { 0.0, 0.0, 0.0 },
        .nokta_karakter = { 'O', 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 8: f(x) = x * sin(x), Aralik: [0, pi]                         */
/* -------------------------------------------------------------------- */
void soru_8_coz(void)
{
    const char *baslik = "SORU 8: f(x) = x*sin(x) ile x Ekseni Arasi [0, pi]";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  [0, pi] araliginda x >= 0 VE sin(x) >= 0  =>  f(x) = x*sin(x) >= 0\n");
    printf("  Fonksiyon butun aralikta pozitif, mutlak degere gerek yok.\n\n");
    printf("  Parcali integral (IBP): u = x, dv = sin(x)dx\n");
    printf("    du = dx,  v = -cos(x)\n");
    printf("  Alan = [-x*cos(x)]_{0}^{pi} + int_{0}^{pi} cos(x)dx\n");
    printf("       = pi + 0 = pi\n\n");

    double alan = simpson_integral(soru8_fonksiyon, 0.0, PI, SIMPSON_ADIM);

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{0}^{pi} x*sin(x) dx = %12.10f\n", alan);
    printf("    Analitik Sonuc           = %12.10f  (pi)\n\n", PI);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -0.2, .x_max = 3.5,
        .y_min = -0.5, .y_max = 3.5,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru8_fonksiyon, '*' }
        },
        .alan_sayisi = 1,
        .alanlar = {
            { 0.0, PI, soru8_fonksiyon, sifir_fonk }
        },
        .nokta_sayisi = 2,
        .nokta_x = { 0.0, PI },
        .nokta_y = { 0.0, 0.0 },
        .nokta_karakter = { 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 9: f(x) = x^4 + 6x^3 + 9x^2 = x^2(x+3)^2                    */
/* -------------------------------------------------------------------- */
void soru_9_coz(void)
{
    const char *baslik = "SORU 9: f(x)=x^2(x+3)^2 ile x Ekseni Kapali Bolge";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  f(x) = x^4 + 6x^3 + 9x^2 = x^2 * (x+3)^2\n\n");
    printf("  Carpanlara ayirma: x^2 >= 0 (her zaman) ve (x+3)^2 >= 0 (her zaman)\n");
    printf("  => f(x) >= 0 BUTUN x degerleri icin.\n\n");
    printf("  Kokler (cift kokler): x = -3 (cift kok), x = 0 (cift kok)\n");
    printf("  Kapali bolge: [-3, 0]\n\n");
    printf("  Alan = int_{-3}^{0} x^2(x+3)^2 dx\n");
    printf("       = [x^5/5 + 3x^4/2 + 3x^3]_{-3}^{0} = 81/10 = 8.1\n\n");

    double alan = simpson_integral(soru9_fonksiyon, -3.0, 0.0, SIMPSON_ADIM);

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{-3}^{0} x^2(x+3)^2 dx = %12.10f\n", alan);
    printf("    Analitik Sonuc             = %12.10f\n\n", 81.0/10.0);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -3.5, .x_max = 0.5,
        .y_min = -2.0, .y_max = 12.0,
        .fonk_sayisi = 1,
        .fonksiyonlar = {
            { soru9_fonksiyon, '*' }
        },
        .alan_sayisi = 1,
        .alanlar = {
            { -3.0, 0.0, soru9_fonksiyon, sifir_fonk }
        },
        .nokta_sayisi = 2,
        .nokta_x = { -3.0, 0.0 },
        .nokta_y = { 0.0, 0.0 },
        .nokta_karakter = { 'O', 'O' }
    };
    ascii_grafik_ciz(&g);
}

/* -------------------------------------------------------------------- */
/*  SORU 10: y = 7e^x, y = 3 - 2e^x, y ekseni (x = 0)                */
/* -------------------------------------------------------------------- */
void soru_10_coz(void)
{
    const char *baslik = "SORU 10: y=7e^x, y=3-2e^x ve y Ekseni Arasindaki Alan";

    printf("================================================================\n");
    printf("  %s\n", baslik);
    printf("================================================================\n\n");

    double ln3 = log(3.0);

    printf("  MATEMATIKSEL COZUM:\n");
    printf("  Kesim noktasi: 7e^x = 3 - 2e^x  =>  9e^x = 3  =>  x = -ln(3)\n");
    printf("    x = -ln(3) = %.10f\n\n", ln3);
    printf("  y ekseni (x = 0) sag sinir.\n\n");
    printf("  [-ln3, 0] araliginda 7e^x >= 3-2e^x (7e^x ustte)\n\n");
    printf("  Alan = int_{-ln3}^{0} [(7e^x) - (3-2e^x)] dx\n");
    printf("       = int_{-ln3}^{0} (9e^x - 3) dx\n");
    printf("       = [9e^x - 3x]_{-ln3}^{0} = 6 - 3*ln(3)\n\n");

    double alan = simpson_fark(soru10_ust, soru10_alt, -ln3, 0.0, SIMPSON_ADIM);
    double analitik = 6.0 - 3.0 * ln3;

    printf("  SAYISAL SONUCLAR:\n");
    printf("    int_{-ln3}^{0} [(7e^x)-(3-2e^x)] dx = %12.10f\n", alan);
    printf("    Analitik Sonuc                        = %12.10f\n\n", analitik);

    GrafikAyarlari g = {
        .baslik = baslik,
        .x_min = -1.5, .x_max = 0.5,
        .y_min = -1.0, .y_max = 8.0,
        .fonk_sayisi = 2,
        .fonksiyonlar = {
            { soru10_ust, '*' },
            { soru10_alt, '+' }
        },
        .alan_sayisi = 1,
        .alanlar = {
            { -ln3, 0.0, soru10_ust, soru10_alt }
        },
        .nokta_sayisi = 3,
        .nokta_x = { -ln3, 0.0, 0.0 },
        .nokta_y = { 7.0*exp(-ln3), 7.0, 3.0-2.0 },
        .nokta_karakter = { 'O', 'A', 'B' }
    };
    ascii_grafik_ciz(&g);
}

/* ========================================================================
 *  BOLUM 6 : KONSOL MENUSU VE ANA PROGRAM (main)
 * ======================================================================== */

/**
 * Konsol menusunu ekrana yazdirir.
 * Kullaniciya 1-10 arasi soru secenegi ve 0 (cikis) sunar.
 */
void menu_goster(void)
{
    printf("\n");
    printf("================================================================\n");
    printf("       MATEMATIK-II PROJE ODEVI\n");
    printf("    Belirli Integral ile Alan Hesaplama\n");
    printf("================================================================\n");
    printf("\n");
    printf("   [ 1]  f(x) = x^2 - 4                Aralik: [-2, 3]\n");
    printf("   [ 2]  f(x) = sin(x)                 Aralik: [-pi, pi]\n");
    printf("   [ 3]  y = sin(x), y = cos(x)        Aralik: [0, pi/2]\n");
    printf("   [ 4]  y = sqrt(x), y = 3            Aralik: [0, 9]\n");
    printf("   [ 5]  y = x^2 - 2, y = 3x - x^2    Aralik: [-0.5, 2]\n");
    printf("   [ 6]  y = x^2, Teget: y = 4x - 4   Aralik: [0, 2]\n");
    printf("   [ 7]  y = x(x^2 - 1)                Aralik: [-1, 1]\n");
    printf("   [ 8]  f(x) = x * sin(x)             Aralik: [0, pi]\n");
    printf("   [ 9]  f(x) = x^2(x+3)^2             Aralik: [-3, 0]\n");
    printf("   [10]  y = 7e^x, y = 3-2e^x         Aralik: [-ln3, 0]\n");
    printf("\n");
    printf("   [ 0]  CIKIS\n");
    printf("\n");
    printf("================================================================\n");
}

/**
 * Kullanicidan bir tamsayi secim alir.
 * Gecersiz giris durumlarini kontrol eder.
 *
 * @return Kullanicinin sectigi menu numarasi (0-10)
 */
int secim_al(void)
{
    int secim;
    printf("   Seciminiz [0-10]: ");
    scanf("%d", &secim);

    /* Gecerlilik kontrolu */
    if (secim < 0 || secim > 10) {
        printf("\n   *** GECERSIZ SECIM! Lutfen 0 ile 10 arasinda bir sayi girin. ***\n");
        return -1;
    }
    return secim;
}

/**
 * Ana program fonksiyonu.
 * Menu-dongusu icerir: kullanici 0 girene kadar tekrar sorar.
 * Her secim switch-case ile ilgili cozum fonksiyonuna yonlendirilir.
 */
int main(void)
{
    int secim;

    printf("\n  *** Matematik-II Proje Odevi Programina Hosgeldiniz! ***\n");
    printf("  *** Derleme: gcc matematik2_proje.c -o matematik2_proje -lm ***\n");

    do {
        menu_goster();
        secim = secim_al();

        switch (secim) {
            case 0:  printf("\n  Program sonlandirildi. Tesekkurler!\n\n");  break;
            case 1:  soru_1_coz();   break;
            case 2:  soru_2_coz();   break;
            case 3:  soru_3_coz();   break;
            case 4:  soru_4_coz();   break;
            case 5:  soru_5_coz();   break;
            case 6:  soru_6_coz();   break;
            case 7:  soru_7_coz();   break;
            case 8:  soru_8_coz();   break;
            case 9:  soru_9_coz();   break;
            case 10: soru_10_coz();  break;
            default: break; /* gecersiz secim, dongu tekrar */
        }

        /* Soru cozumden sonra kullaniciya devam etme secenegi sun */
        if (secim > 0 && secim <= 10) {
            printf("  [Devam etmek icin Enter'a basin...]");
            /* Girdi tamponunu temizle */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            getchar(); /* Enter tusunu bekle */
        }
    } while (secim != 0);

    return 0;
}
