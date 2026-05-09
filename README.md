# Matematik-II: Belirli İntegral ile Alan Hesabı Projesi

[cite_start]Bu proje, **Kırklareli Üniversitesi** Yazılım Mühendisliği Bölümü Matematik-II dersi kapsamında geliştirilmiştir[cite: 1, 31]. [cite_start]Proje, 10 farklı matematiksel fonksiyonun ve eğrinin sınırlandırdığı alanları **Simpson 1/3 Sayısal İntegral Yöntemi** kullanarak hesaplayan modüler bir C programıdır[cite: 32].

##  Proje Özellikleri

* [cite_start]**Dil:** Saf C Programlama Dili[cite: 5].
* [cite_start]**Algoritma:** Yüksek doğruluk payına sahip **Simpson 1/3 Kuralı**[cite: 32].
* **Görselleştirme:** Ek kütüphane gerektirmeyen, terminal tabanlı dinamik **ASCII Grafik Motoru**.
* [cite_start]**Mühendislik Yaklaşımı:** "Clean Code" (Temiz Kod) prensiplerine uygun, struct yapıları ile modüler mimari[cite: 6].
* **Karşılaştırma:** Nümerik sonuçların analitik çözümlerle otomatik kıyaslanması.

##  Ele Alınan Problemler

Program aşağıdaki 10 temel matematiksel problemi çözmektedir:

1.  [cite_start]Parabol ve x-ekseni arası alan ($f(x) = x^2 - 4$)[cite: 18, 34].
2.  [cite_start]Trigonometrik alan hesabı ($f(x) = \sin(x)$)[cite: 19, 35].
3.  [cite_start]İki trigonometrik eğri arası alan ($\sin(x)$ ve $\cos(x)$)[cite: 20, 36].
4.  [cite_start]Karekök fonksiyonu ve yatay doğru arası alan ($y = \sqrt{x}$, $y = 3$)[cite: 23, 37].
5.  [cite_start]İki parabol arasında kalan bölge ($y = x^2-2$ ve $y = 3x-x^2$)[cite: 24, 38].
6.  [cite_start]Parabol ve teğet doğrusu arası alan ($y = x^2$ ve $x=2$ teğeti)[cite: 25, 39].
7.  [cite_start]Polinom fonksiyonu kapalı bölgeleri ($y = x^3 - x$)[cite: 27, 40].
8.  [cite_start]Çarpım fonksiyonu integrali ($f(x) = x \cdot \sin(x)$)[cite: 28, 41].
9.  [cite_start]4. derece polinom ve teğet noktaları ($f(x) = x^2(x+3)^2$)[cite: 29, 42].
10. [cite_start]Üstel fonksiyonlar arası alan ($y = 7e^x$ ve $y = 3-2e^x$)[cite: 30, 43].

##  Kurulum ve Çalıştırma

Projenin bilgisayarınızda çalışması için bir C derleyicisinin (GCC önerilir) yüklü olması gerekir.

1.  Depoyu klonlayın:
    ```bash
    git clone [https://github.com/kullanici_adiniz/Matematik2-Integral-Projesi.git](https://github.com/kullanici_adiniz/Matematik2-Integral-Projesi.git)
    ```
2.  Proje dizinine gidin:
    ```bash
    cd Matematik2-Integral-Projesi
    ```
3.  Matematik kütüphanesini bağlayarak derleyin:
    ```bash
    gcc matematik2_proje.c -o integral_hesapla -lm
    ```
4.  Programı çalıştırın:
    ```bash
    ./integral_hesapla
    ```

##  Örnek Çıktı (ASCII Grafik)

Program, her hesaplama sonrası terminale aşağıdakine benzer bir görselleştirme basmaktadır:

```text
    --- GRAFIK (y ekseni dikey, x ekseni yatay) ---
    Alandaki taralı bolge '#' karakteri ile gosterilmistir.

      5.0 |          *
          |         *#*
      0.0 |----------#----------
          |        *###*
     -5.0 |      *#######*
