=======LOTY======== Projekt symuluje planowanie trasy lotu samolotu w dwuwymiarowym polu wektorowym reprezentującym pole wiatru. Program generuje:

siatkę wektorową pola wiatru,
plan lotu z czasem przelotu między punktami (bez uwzględnienia wiatru),
trajektorię lotu uwzględniającą wpływ wiatru na czas przelotu.
Struktura projektu Główna funkcja programu:

Wczytuje dane z plików in.txt i track.txt,
Tworzy obiekt klasy Generator,
Generuje siatkę wiatru,
Tworzy plan lotu oraz trajektorię z uwzględnieniem wiatru.
generator.cpp / generator.h Klasa Generator odpowiada za: Tworzenie pola wektorowego (generuj_siatke), generowanie planu lotu (generuj_plan_lotu), generowanie rzeczywistej trajektorii (generuj_trajectorie). Wektor wiatru w siatce jest generowany według wzoru:

v = A * (e / B) * r * e^(-r / B) gdzie r to odległość punktu od środka pola, a A i B to parametry wpływające na siłę i zasięg wiatru.

wektor.cpp / wektor.h Pomocnicza klasa Wektor, przechowująca wartości składowych vx i vy.

Pliki wejściowe in.txt Parametry siatki wiatru:

xd xg Nx yd yg Ny

gdzie: xd, xg: zakres osi X yd, yg: zakres osi Y Nx, Ny: liczba punktów siatki w danym kierunku

track.txt Lista punktów trasy:

etykieta x y

Przykład: dep 0 0 a1 200 300 ...

Pliki wyjściowe plan.txt: Plan lotu z czasami przelotu pomiędzy punktami bez uwzględnienia wiatru.

Format: etykieta x y czas[min]

trajectory.txt: Trajektoria lotu z uwzględnieniem wiatru

=======DEKODER GRIB======== Ten program służy do dekodowania wiadomości w formacie GRIB (GRIdded Binary) – standardzie binarnym używanym do przekazywania danych meteorologicznych, takich jak pola ciśnienia, temperatury czy wiatru. Plik wejściowy zawiera dane w formacie GRIB, a wyniki dekodowania są zapisywane do pliku tekstowego output.txt.

Każda wiadomość GRIB zawiera kilka sekcji. Program identyfikuje poszczególne sekcje i wypisuje zawarte w nich informacje:

Sekcje: Sekcja 1 – PDS (Product Definition Section) Zawiera:

Długość sekcji
Flagi obecności GDS i BMS
Typ poziomu (np. ciśnienia)
Ciśnienie
Data i godzina wygenerowania danych
Sekcja 2 – GDS (Grid Description Section) Zawiera:

Wymiary siatki (np. liczba wierszy)
Początkowe i końcowe współrzędne szerokości i długości geograficznej
Przyrosty współrzędnych
Inne wartości binarne i przeliczenia (część wypisywana w pętli)
Sekcja 4 – BDS (Binary Data Section) Zawiera:

Wartość referencyjna R
Współczynnik skali
Liczbę bitów na wartość
wartości danych meteorologicznych
Sekcja 5 – End Section

Zawiera stałą wartość tekstową "7777" (znak końca wiadomości).
Jak działa program Plik wejściowy: do_dekodera_dwa_komunikaty.grib

Program otwiera plik binarny i przeszukuje go w poszukiwaniu nagłówków "GRIB".

Po wykryciu wiadomości odczytuje jej długości, tworzy obiekt Decoder, który zawiera metody do odczytu danych binarnych. Wywołuje kolejno: DecoderSec1 DecoderSec2 DecoderSec4 DecoderSec5

Wynik jest zapisywany do output.txt.
