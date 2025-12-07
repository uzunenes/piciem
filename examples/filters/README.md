# Filters Example

Bu örnek `lpgm_convolve()` fonksiyonuyla Gaussian blur uygular.

## Çalıştırma

```bash
make
./gaussian_blur.out ../pgm_io/lena_ascii.pgm
```

## Ödev: Kendi Filtrenizi Yazın

`gaussian_blur.c` örneğini inceleyin ve aşağıdaki filtreleri kendiniz yazın:

### 1. Mean Filter (Ortalama)
Tüm komşu piksellerin ortalamasını alır.

```c
const float mean_3x3[9] = {
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f
};
```

### 2. Sharpen (Keskinleştirme)
Kenarları belirginleştirir.

```c
const float sharpen_3x3[9] = {
     0.0f, -1.0f,  0.0f,
    -1.0f,  5.0f, -1.0f,
     0.0f, -1.0f,  0.0f
};
```

### 3. Emboss (Kabartma)
3D kabartma efekti verir.

```c
const float emboss_3x3[9] = {
    -2.0f, -1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,
     0.0f,  1.0f,  2.0f
};
```

### 4. Laplacian (Kenar Algılama)
İkinci türev tabanlı kenar algılama.

```c
const float laplacian_3x3[9] = {
     0.0f,  1.0f,  0.0f,
     1.0f, -4.0f,  1.0f,
     0.0f,  1.0f,  0.0f
};
```

### 5. 5x5 Gaussian Blur
Daha güçlü yumuşatma için 5x5 kernel kullanın.

```c
const float gaussian_5x5[25] = {
    1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f,
    4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f,
    6.0f/256.0f, 24.0f/256.0f, 36.0f/256.0f, 24.0f/256.0f, 6.0f/256.0f,
    4.0f/256.0f, 16.0f/256.0f, 24.0f/256.0f, 16.0f/256.0f, 4.0f/256.0f,
    1.0f/256.0f,  4.0f/256.0f,  6.0f/256.0f,  4.0f/256.0f, 1.0f/256.0f
};

// Kullanım: lpgm_convolve(&pgm.im, gaussian_5x5, 5);
```

## İpuçları

- Kernel toplamı 1 olmalı (normalleştirme)
- Kernel boyutu tek sayı olmalı (3, 5, 7, ...)
- Kenar algılama kernel'larında toplam 0 olabilir
