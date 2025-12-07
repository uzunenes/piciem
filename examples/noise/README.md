# Noise & Denoise Example

Bu örnek gürültü ekleme ve temizleme işlemlerini gösterir.

## Çalıştırma

```bash
make
./denoise_median.out ../pgm_io/lena_ascii.pgm
```

Çıktılar:
- `output_noisy.pgm` - Salt & pepper gürültü eklenmiş
- `output_denoised.pgm` - Median filter ile temizlenmiş

## Ödev: Karşılaştırma Yapın

### 1. Mean Filter ile Denoise
`lpgm_convolve()` kullanarak mean filter uygulayın ve median filter ile karşılaştırın.

```c
const float mean_3x3[9] = {
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
    1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f
};

denoised = lpgm_convolve(&noisy, mean_3x3, 3);
```

**Soru**: Hangisi salt & pepper gürültüsünü daha iyi temizliyor? Neden?

### 2. Farklı Gürültü Oranları
`density` parametresini değiştirin: 0.01, 0.05, 0.10, 0.20

```c
noisy = lpgm_add_salt_pepper_noise(&pgm.im, 0.10f);  /* 10% noise */
```

### 3. Farklı Median Boyutları
3x3, 5x5, 7x7 median filter karşılaştırın.

```c
denoised_3 = lpgm_median_filter(&noisy, 3);
denoised_5 = lpgm_median_filter(&noisy, 5);
denoised_7 = lpgm_median_filter(&noisy, 7);
```

**Soru**: Büyük kernel neyi iyileştiriyor, neyi kötüleştiriyor?

### 4. Gaussian Noise (İleri Seviye)
Kendi Gaussian noise fonksiyonunuzu yazın:

```c
/* Box-Muller transform ile Gaussian random */
float gaussian_random(float mean, float stddev)
{
    float u1 = (float)rand() / RAND_MAX;
    float u2 = (float)rand() / RAND_MAX;
    float z = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
    return mean + stddev * z;
}
```

## Beklenen Sonuçlar

| Yöntem | Salt & Pepper | Gaussian Noise |
|--------|---------------|----------------|
| Mean Filter | Orta | İyi |
| Median Filter | Çok İyi | Orta |
| Gaussian Blur | Kötü | Çok İyi |
