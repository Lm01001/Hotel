#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

class Gosc {
	string imie, email, nazwisko;
	double nr_tel, id_goscia;
public:
	Gosc(const string& imie, const string& nazwisko, const string& email, double nr_tel, double id_goscia) 
		: imie(im), nazwisko(nazw), email(email), nr_tel(nr_tel), id_goscia(id_goscia) {}

	double get_id_goscia() const {
		return id_goscia;
	}
	vector<string> zobacz_oferty_pokoi() {
	}
	void zrob_rezerwacje(int id_pokoju, const string& poczatek, const string& koniec) {
	}
	void anuluj_rezerwacje(int id_rezerwacji) {
	}
};

class Rachunek {
	double kwota;
	string data_wystawienia, data_platnosci;
public:
	utworz_rachunek(const string& data_wystawienia, const string& data_platnosci) {
	}
	zaplac(double& kwota) {
	}
};

class Administrator_systemu {
	double id_administratora;
	void zarzadzaj_permisjami(const id_administratora, string login, string haslo, string permisje) { //dokonczyc koncowka
		cout << "Podaj ID administratora: " << endl;
		permisje = "rrr";
		if (!id_administratora)
			return;
		cout << "Podaj login: " << endl;
		if (!login)
			return;
		cout << "Podaj haslo: " << endl;
		if (!haslo)
			return;
		cout << "Zmiana permisji (aktualne r-r-r, podawac bez " - "):" << endl
			cin >> permisje;
		cout << endl << "Aktualne permisje po zmianie: " << permisje[0] << "-" << permisje[1] << "-" << permisje[2] << endl;
		cout << "Zmiany wprowadzone i zapisane pomyslnie!" << endl;
		return;
	}
	zarzadzaj_kontami(string id_konta_goscia) {
	}
};

class Rezerwacja{
	double id_rezerwacji;
	string poczatek_rezerwacji, koniec_rezerwacji;
public:
	void utworz_rezerwacje(string standard_pokoju, string imie_goscia, string nazwisko_goscia, const string& poczatek_rezerwacji, const string& koniec_rezerwacji) { // czy const przy dacie???
	}
	void usun_rezerwacje(const double& id_rezerwacji) {
	}
	void aktualizuj_rezerwacje(const double& id_rezerwacji) {
	}
};

class I_menadzer_rezerwacji {
public:
	virtual void stworzRezerwacje(int id_goscia, Rezerwacja rezerwacja) = 0;
	virtual void anulujRezerwacje(int id_rezerwacji) = 0;
	virtual void aktualizujRezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) = 0;
	virtual vector<Rezerwacja> wyswietlRezerwacje(int id_goscia) = 0;
	virtual ~I_menadzer_rezerwacji() = default;  //wirtualny destruktor
};

class Menadzer_rezerwacji : public I_menadzer_rezerwacji {	  //klucz: id_goscia, hash-table na wartosci klucz-gosc
	unordered_map<int>, vector<Rezerwacja>> rezerwacje-mapa;  //z biblioteki <unordered_map> funckja dzialajaca jak powyzej
public:
	void stworzRezerwacje(int id_goscia, Rezerwacja rezerwacja) override {} //do nadpisywania i wychwytywania bledow dot klasy bazowej    !!!!!!!
	void anulujRezerwacje(int id_rezerwacji) override {}
	void aktualizujRezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) override {}
	vector<Rezerwacja> wyswietlRezerwacje(int id_goscia) override {}
};

class Pokoj{
	int numer;
	bool dostepnosc;
public:
	string standard;
	float cena;
	bool ustaw_status(const int& numer) {
	}
	wyswietl_informacje_pokoju(const int& numer, int id_rezerwacji) {
	}
	pokaz_dostepnosc_standardu(const bool& dostepnosc, int& numer) {
	}
};

class Pracownik {
protected:
	string imie, nazwisko, rola;
	double id_pracownika;
public:
	Pracownik (int im) : imie (im) {}            //konstruktory, utworzone w celu umozliwienia klasie menadzer_hotelu
	Pracownik (int nazw) : nazwisko (nazw) {}   //dziedziczenie elementow protected 
	Pracownik (int rol) : rola (rol) {}
	Pracownik (int id_prac) : id_pracownika (id_prac) {}
	aktualizuj_dostepnosc(int id_rezerwacji, int numer_pokoju) {
	}
	zarejstruj_godziny_pracy(double poczatek_pracy, double koniec_pracy) {//czy trzeba wgl??????
	}
};

class Menadzer_hotelu : public Pracownik{
	weryfikuj_rezerwacje(const int& id_rezerwacji, string status_rezerwacji) {
	}
public:
	Menadzer_hotelu (int im) : Pracownik (im) {}		   //dziedziczenie elementow protected
	Menadzer_hotelu (int nazw) : Pracownik (nazw) {}
	Menadzer_hotelu (int rol) : Pracownik (rol) {}
	Menadzer_hotelu (int id_prac) : Pracownik (id_prac) {}
	void utworz_raport(double& id_pracownika, double& poczatek_pracy, double& koniec_pracy) {
	}
	void zarzadzaj_personelem(double& id_pracownika, string aktualne_zadanie) {
	}
};

class System_hotelowy {
	double wersja_oprogramowania_sys;             //czy cos ponizej??
	void konfiguruj_ustawienia_systemowe() {
	}
protected:
	void sprawdz_logi_systemu(double czas_zdarzenia, double id_uzytkownika, string okolicznosci_zdarzenia, string poziom_logu) { //idk czy trzeba???
	}
	bool zamelduj_goscia(double id_goscia) {
	}
	bool wymelduj_goscia(double id_goscia, double dodatkowe_oplaty) {
	}
};

class Hotel {
public:
	string nazwa, adres;
	float liczba_gwiazdek;
	int wyswietl_informacje_o_hotelu(const float& liczba_gwiazdek, const string& nazwa, const string& adres) {
	}
	int wyswietl_dostepne_pokoje(string standard_pokoju, int liczba_pokoi) {
	}
};

int main()
{
	return 0;
}
