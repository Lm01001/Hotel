#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

/*class Gosc{
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

class Rachunek{
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
		cin >> id_administratora;
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
};*/

class Rezerwacja{
	double id_rezerwacji;
	string poczatek_rezerwacji, koniec_rezerwacji, dane;
public:
	bool poprawna_data(const string& data){
    	if (data.size() != 10) return false;
    	if ((data[2] != '-' && data[2] != '.') || (data[5] != '-' && data[5] != '.')) 
        	return false;
    	for (int i = 0; i < data.size(); ++i) 
		{
       		if (i == 2 || i == 5) continue;  // Skip the separator
        		if (!isdigit(data[i])) return false;
    	}
    	return true;
	}
	Rezerwacja(){ //pseudolosowe generowanie id rezerwacji. zakres 1-1000
        srand(time(0));
        id_rezerwacji=rand()%1000+1; 
    }
	void utworz_rezerwacje(){   //dziala 
		string standard_pokoju, imie_goscia, nazwisko_goscia; 
		int i = 0;
		cout<<"Podaj imie: "; cin >> imie_goscia;
		cout<<"Podaj nazwisko: "; cin >> nazwisko_goscia;
		for(;;){
			cout<<"Wybierz standard pokoju('standard', 'studio' lub 'premium'): "; cin >> standard_pokoju;
			transform(standard_pokoju.begin(), standard_pokoju.end(), standard_pokoju.begin(), ::tolower);
			if(standard_pokoju!="standard" && standard_pokoju!="studio" && standard_pokoju!="premium")
			{
				cout<<"Taki standard nie istnieje. Chcesz wybrać ponownie, podaj 1.\n";
				cin>>i;
				if(i!=1)
	           		return;
				else	
					continue;
			}else	
				break;
		}
		for (;;) 
		{
        	cout << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin >> poczatek_rezerwacji;
        	if (!poprawna_data(poczatek_rezerwacji)) 
           		cout<<endl<<"Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR"<<endl;
         	else 
            	break;
        }
		for (;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin >> koniec_rezerwacji;
        	if (!poprawna_data(koniec_rezerwacji))
            	cout<<endl<<"Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR"<<endl;
        	else 
            	break;
        }
		cout<<"Rezerwacja utworzona pomyslnie!"<<endl;
		ostringstream oss;
        	oss<<left<<setw(20)<<id_rezerwacji<<setw(15)<<(imie_goscia.size()>18 ? imie_goscia.substr(0, 17)+"." : imie_goscia)
        	<<setw(20)<<(nazwisko_goscia.size()>18 ? nazwisko_goscia.substr(0, 17)+"." : nazwisko_goscia)<<setw(20)<<standard_pokoju
			<<setw(20)<<poczatek_rezerwacji<<setw(20)<<koniec_rezerwacji;
        	dane+=oss.str()+"\n";
			cout<<left;  
			cout<<setw(20)<<"ID Rezerwacji"<<setw(15)<<"Imie"<<setw(20)<<"Nazwisko"<<setw(20)<<"Standard"<<setw(20)<<"Poczatek"<<setw(20)<<"Koniec"<<endl;
			cout<<string(110, '-')<<endl;
			cout<<dane;
	}
	void usun_rezerwacje(const double& id_rezerwacji){

	}
	void aktualizuj_rezerwacje(const double& id_rezerwacji){

	}
};
		

/*class I_menadzer_rezerwacji {
public:
	virtual void stworzRezerwacje(int id_goscia, Rezerwacja rezerwacja) = 0;
	virtual void anulujRezerwacje(int id_rezerwacji) = 0;
	virtual void aktualizujRezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) = 0;
	virtual vector<Rezerwacja> wyswietlRezerwacje(int id_goscia) = 0;
	virtual ~I_menadzer_rezerwacji() = default;  //wirtualny destruktor
};

class Menadzer_rezerwacji : public I_menadzer_rezerwacji {	  //klucz: id_goscia, hash-table na wartosci klucz-gosc
	unordered_map<int, vector<Rezerwacja>> rezerwacje-mapa;  //z biblioteki <unordered_map> funckja dzialajaca jak powyzej
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

class Pracownik{
protected:
	string imie, nazwisko, rola;
	double id_pracownika;
public:
	Pracownik (int im) : imie (im){}            //konstruktory, utworzone w celu umozliwienia klasie menadzer_hotelu
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

class System_hotelowy{
	double wersja_oprogramowania_sys;             //czy cos ponizej??
	void konfiguruj_ustawienia_systemowe() {

	}
protected:
	void sprawdz_logi_systemu(bool awaria, double czas_zdarzenia, double id_uzytkownika, string okolicznosci_zdarzenia,
		 string poziom_logu) { //idk czy trzeba???
		if (!awaria)
		{
			cout << "Brak awarii. Konczenie procesu." << endl;
			return 0;
		}
		else
		{
			cout << "Wystapienie awarii !!!" <<endl;
			cout << "czas: " <<czas_zdarzenia<<" " << okolicznosci zdarzenia << endl << "uzytkownik: " << id_uzytkownika <<
				" poziom logu: " << poziom_logu << endl;
			return 0;
		}
	}
	int zamelduj_goscia(double id_goscia, int id_rezerwacji) { //mozna by zrobic unordered map i dac
	unordered_map<double id-goscia, int id_rezerwacji> gosc-rezerwacja; //gosc jako klucz id rezerwacji sprawdzic  !!!!!!
	if (!id_goscia || !id_rezerwacji)  
		return 0;
	else
		return 1;
	}
	int wymelduj_goscia(double id_goscia, double dodatkowe_oplaty) {
		if(dodatkowe_oplaty != 0)
		{
			cout << "Prosba o uregulowanie naleznej kwoty. " << endl;
			//mozna sprobowac w rachunku klasie dac static i uzyc metody
		}
		return 1;
	}
};*/

/*class Hotel{ //klasa wstepnie skonczona - dziala
public:
	const static string nazwa, adres;
	const static float l_gwiazdek;
	int liczba_pokoi[3];
	int wyswietl_informacje_o_hotelu(const float& l_gwiazdek, const string& nazwa, const string& adres) {  // metoda dziala
		cout << "Nazwa: " << nazwa << endl;
		cout << "Adres: " << adres << endl;
		cout << "Liczba gwiazdek: ";
		for(int i = 0; i < l_gwiazdek; i++)
			cout << "*";
		cout << endl << endl;
		return 0;
	}
	int wyswietl_dostepne_pokoje() {
	    int i=0;
		srand(time(NULL));
		for (int i = 0; i < 3; ++i) {
            liczba_pokoi[i] = rand() % 123 + 1;
        }
	    wybor:
	    cout << "Podaj standard pokoju, gdzie 0-standard, 1-studio i 2-premium: " << endl;
	    cin >> i;
		switch(i){
			case 0:
				cout << "Liczba dostepnych pokoi 'standard': " << liczba_pokoi[0] << endl;
				return 0;
			case 1:
				cout << "Liczba dostepnych pokoi 'studio': " << liczba_pokoi[1] << endl;
				return 0;
			case 2:
				cout << "Liczba dostepnych pokoi 'premium': " << liczba_pokoi[2] << endl;
				return 0;
			default:
				cout << "Podano niepoprawną wartość! Chcesz wybrac ponownie wpisz 1." << endl;
	        	cin >> i; 
	        	if(i == 1)
	            	goto wybor; 
				else
	            	return 0;
		};
	}
};*/

int main()
{
	/*Hotel h;
	h.wyswietl_informacje_o_hotelu(4, "nazwa", "ul.coscos");
	cout<<endl<<endl;
	h.wyswietl_dostepne_pokoje();*/
	
	Rezerwacja r;
	r.utworz_rezerwacje();
	return 0;
}
