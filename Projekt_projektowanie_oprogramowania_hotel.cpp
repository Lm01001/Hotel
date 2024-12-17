#include <iostream>   // zrobione:  hotel,  gosc, rezerwacja
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
#include <sqlite3.h>
#include <chrono>
#include <thread>

using namespace std; // kompilacja:  g++ -std=c++11 -o hotel_program Projekt_projektowanie_oprogramowania_hotel.cpp -lsqlite3

void czekaj(int s) 
{
    this_thread::sleep_for(chrono::seconds(s)); // Wstrzymanie na 'sekundy' programu
}


class Gosc
{
	string imie, email, nazwisko, dane;
	double nr_tel, id_goscia;
	sqlite3* db;
public:       //sprawdzic czy generowanie id dla goscia potrzebne !!!!!!!!!!!
	Gosc() //pseudolosowe generowanie id goscia. zakres 1-1000
	{ 
        srand(time(0));
        id_goscia = rand() % 5000 + 1; //mamy te dane podawane w rezerwacji 
    }  
	Gosc(const string& db_path)
	{
        if(sqlite3_open(db_path.c_str(), &db))
		{
            cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
            db = nullptr;
			exit(1);
        }
    }
	vector<string> zobacz_oferty_hoteli(const string& panstwo = "", int min_gwiazdek = 0) 
	{
		vector<string> oferty;
		cout << endl << "\t---Wyswietlanie dostepnych hoteli---" << endl;
		if(!db)
		{
            cerr << "Baza danych nie jest otwarta!" << endl;
            return oferty;
        }
        string sql = "SELECT nazwa_hotelu, ilosc_gwiazdek, miasto, panstwo FROM hotele WHERE 1=1";
        if(!panstwo.empty())
          	sql += " AND panstwo = '" + panstwo + "'";
        if(min_gwiazdek>0)
            sql += " AND ilosc_gwiazdek >= " + to_string(min_gwiazdek);
        auto callback = [](void* oferty, int argc, char** argv, char** azColName) -> int // Funkcja callback do przetwarzania wyników
		{
            vector<string>* wyniki_vector = static_cast<vector<string>*>(oferty);
            string wynik = "Hotel: " + string(argv[0]) + ", Gwiazdki: " + string(argv[1]) + ", Miasto: " + string(argv[2]) + ", Panstwo: " + string(argv[3]);
            wyniki_vector->push_back(wynik);
            return 0;
        };

       char* error = nullptr;
        if(sqlite3_exec(db, sql.c_str(), callback, &oferty, &error)!=SQLITE_OK)
		{
            cerr<<"Błąd podczas wykonywania zapytania: "<<error<< endl;
            sqlite3_free(error);
        }
		return oferty;
	}
	~Gosc() //destruktor
	{ 
        if(db) 
        	sqlite3_close(db);
    }
};

class Rezerwacja
{
	string poczatek_rezerwacji, koniec_rezerwacji, dane;
	double id_rezerwacji;
public:
	bool poprawna_data(const string& data)
	{
    	if(data.size() != 10) return false;
    	if((data[2] != '-' && data[2] != '.') || (data[5] != '-' && data[5] != '.')) 
        	return false;
    	for(int i = 0; i < data.size(); ++i) 
		{
       		if (i == 2 || i == 5) 
				continue;  // pominiecie "oddzielnika"
        	if (!isdigit(data[i])) 
				return false;
    	}
    	return true;
	}


	Rezerwacja() //pseudolosowe generowanie id rezerwacji. zakres 1-1000
	{ 
        srand(time(0));
        id_rezerwacji+=rand()%1000+1; 
    }


	tm string_do_tm(const string& data)
    {
        tm t = {};
        istringstream ss(data);
        ss >> get_time(&t, "%d-%m-%Y");
        return t;
    }

	
	void zapisz_do_bazy(sqlite3* db, const string& imie_goscia, const string& nazwisko_goscia, const string& standard_pokoju) 
	{
        string sql = "INSERT INTO Rezerwacje (id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, poczatek_rezerwacji, koniec_rezerwacji) "
                     "VALUES (" + to_string(id_rezerwacji) + ", '" + imie_goscia + "', '" + nazwisko_goscia + "', '" + standard_pokoju + "', '" + poczatek_rezerwacji + "', '" + koniec_rezerwacji + "');";
    }
	void utworz_rezerwacje() //dziala
	{   
		string standard_pokoju, imie_goscia, nazwisko_goscia;
		int i=0;
		sqlite3* db;
    		if (sqlite3_open("dane_rezerwacji.db", &db) != SQLITE_OK) 	
			{
        		cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
        		return;
    		}
		cout << endl << "\t---Tworzenie rezerwacji---" << endl;
		cout << "Podaj imie: "; cin>>imie_goscia;
		cout << "Podaj nazwisko: "; cin>>nazwisko_goscia;
		
		for(;;){
			cout << "Wybierz standard pokoju('standard', 'studio' lub 'premium'): "; cin>>standard_pokoju;
		transform(standard_pokoju.begin(), standard_pokoju.end(), standard_pokoju.begin(), ::tolower);
		if(standard_pokoju == "standard" || standard_pokoju == "studio" || standard_pokoju == "premium")
			break;
		if(standard_pokoju != "standard" && standard_pokoju != "studio" && standard_pokoju != "premium"){
			cout << "Taki standard nie istnieje. Tworzenie rezerwacji zakończone niepowodzeniem!" << endl;
			cout << "Jesli chcesz wybrac ponownie podaj 1." << endl;
			cin>>i;
		}
		if(i == 1)
				continue;
			else
				exit(1);
		}
		for(;;) 
		{
        	cout << endl << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin>>poczatek_rezerwacji;
        	if(!poprawna_data(poczatek_rezerwacji)) 
           		cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
         	else 
            	break;
        }
		for(;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin>>koniec_rezerwacji;
        	if(!poprawna_data(koniec_rezerwacji))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
        	else 
            	break;
        }
		cout << "Rezerwacja utworzona pomyslnie!" << endl;
		ostringstream oss;
        	oss << left << setw(20) << id_rezerwacji << setw(15) << (imie_goscia.size() > 18 ? imie_goscia.substr(0, 17) + "." : imie_goscia)
        	<< setw(20) << (nazwisko_goscia.size() > 18 ? nazwisko_goscia.substr(0, 17) + "." : nazwisko_goscia) << setw(20) << standard_pokoju
			<< setw(20) << poczatek_rezerwacji << setw(20) << koniec_rezerwacji;
        	dane += oss.str() + "\n";
			cout << left;  
			cout << setw(20) << "ID Rezerwacji" << setw(15) << "Imie" << setw(20) << "Nazwisko" << setw(20) << "Standard" << setw(20) << "Poczatek" << setw(20) 
			<< "Koniec" << endl;
			cout << string(110, '-') << endl;
			cout << dane;
			zapisz_do_bazy(db, imie_goscia, nazwisko_goscia, standard_pokoju);
			
   /*char* err_msg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
        cerr << "Błąd podczas zapisu rezerwacji do bazy danych: " << err_msg << endl;
        sqlite3_free(err_msg);
    } else {
        cout << "Rezerwacja została pomyślnie zapisana w bazie danych." << endl;
    }*/

    sqlite3_close(db);
	}

	~Rezerwacja() {}

	int oblicz_czas_pobytu()
    {
        tm poczatek = string_do_tm(poczatek_rezerwacji);
        tm koniec = string_do_tm(koniec_rezerwacji);

        // Konwersja na czas unixowy (sekundy od 1970-01-01)
        time_t t_poczatek = mktime(&poczatek);
        time_t t_koniec = mktime(&koniec);

        // Różnica w sekundach
        double roznica = difftime(t_koniec, t_poczatek);
        int dni_pobytu = roznica / (60 * 60 * 24);  // Przekształcenie sekund na dni
        return dni_pobytu;
    }

	/*void usun_rezerwacje(){
		cout << endl << "\t---Usuwanie rezerwacji---" << endl;
		id_rezerwacji.clear();
		if(id_rezerwacji != " ")
			cout << "Rezerwacja usunięta pomyślnie."<<endl;
		exit(0);

	}*/
	void aktualizuj_rezerwacje()
	{
		int a;
		cout << endl << "\t---Aktualizowanie rezerwacji---" << endl;
		cout << "Czy na pewno chcesz zaktualizowac calkowicie rezerwacje?  1 - tak, 0 - nie" << endl;
		cin >> a;
		string standard_pokoju, imie_goscia, nazwisko_goscia;
		int i=0;
		if(a == 1)
		{   	
		cout << "Podaj imie: "; cin>>imie_goscia;
		cout << "Podaj nazwisko: "; cin>>nazwisko_goscia;
		cout << "Wybierz standard pokoju('standard', 'studio' lub 'premium'): "; cin>>standard_pokoju;
		transform(standard_pokoju.begin(), standard_pokoju.end(), standard_pokoju.begin(), ::tolower);
		if(standard_pokoju != "standard" && standard_pokoju != "studio" && standard_pokoju != "premium"){
			cout << "Taki standard nie istnieje. Tworzenie rezerwacji zakończone niepowodzeniem!\n";
			exit(1);
		}
		for(;;) 
		{
        	cout << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin>>poczatek_rezerwacji;
        	if(!poprawna_data(poczatek_rezerwacji)) 
           		cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
         	else 
            	break;
        }
		for(;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin>>koniec_rezerwacji;
        	if(!poprawna_data(koniec_rezerwacji))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
        	else 
            	break;
        }
		cout << "Rezerwacja utworzona pomyslnie!" << endl;
		ostringstream oss;
        	oss << left << setw(20) << id_rezerwacji << setw(15) << (imie_goscia.size() > 18 ? imie_goscia.substr(0, 17) + "." : imie_goscia)
        	<< setw(20) << (nazwisko_goscia.size() > 18 ? nazwisko_goscia.substr(0, 17) + "." : nazwisko_goscia) << setw(20) << standard_pokoju
			<< setw(20) << poczatek_rezerwacji << setw(20) << koniec_rezerwacji;
        	dane += oss.str() + "\n";
			cout << left;  
			cout << setw(20) << "ID Rezerwacji" << setw(15) << "Imie" << setw(20) << "Nazwisko" << setw(20) << "Standard" << setw(20) << "Poczatek" << setw(20) 
			<< "Koniec" << endl;
			cout << string(110, '-') << endl;
			cout << dane;
		} else	
			return;

	}
};

class Rachunek
{
	double kwota;
	string data_wystawienia, data_platnosci;
	int id_rezerwacji;
public:
	int getIdRezerwacji() const 
	{
        return id_rezerwacji;
    }
	void wyswietl_id_rezerwacji(const Rezerwacja& rezerwacja) {} // uzycie gettera
	
   	void pobierzDaneZBazy(int id_rezerwacji) 
	{
        sqlite3* db;
        sqlite3_stmt* stmt;
        const char* db_name = "/workspaces/Hotel/Projekt_program_cpp/data/dane_rezerwacji.db";  // Nazwa bazy danych
        if (sqlite3_open(db_name, &db)) 
		{
            cerr << "Nie udało się otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
            return;
        }
   	
        stringstream ss;
        ss << "SELECT id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, poczatek_rezerwacji, koniec_rezerwacji FROM Rezerwacja WHERE id_rezerwacji = " << id_rezerwacji << ";";
        string sql = ss.str();

        // Przygotowanie zapytania
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Błąd podczas przygotowywania zapytania: " << sqlite3_errmsg(db) << endl;
            sqlite3_close(db);
            return;
        }

        // Wykonanie zapytania i przypisanie wyników do zmiennych
        if (sqlite3_step(stmt) == SQLITE_ROW) 
		{
            int id_rezerwacji = sqlite3_column_int(stmt, 0);  // Przypisanie wyników do zmiennych
            const char* imie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* nazwisko = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* standard_pokoju = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* poczatek_rezerwacji = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* koniec_rezerwacji = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            cout << "Dane rezerwacji:" << endl; // Wyświetlenie danych
            cout << "ID Rezerwacji: " << id_rezerwacji << endl;
            cout << "Imię: " << imie << endl;
            cout << "Nazwisko: " << nazwisko << endl;
            cout << "Standard pokoju: " << standard_pokoju << endl;
            cout << "Początek rezerwacji: " << poczatek_rezerwacji << endl;
            cout << "Koniec rezerwacji: " << koniec_rezerwacji << endl;
            this->data_wystawienia = poczatek_rezerwacji;  // data wystawienia rachunku
            this->data_platnosci = koniec_rezerwacji;     // data płatności za pobyt
        }else{
            cout << "Nie znaleziono rezerwacji o ID: " << id_rezerwacji << endl;
        }
        sqlite3_finalize(stmt); // Zwolnienie zasobów, zamkniecie bazy
        sqlite3_close(db);
	}
	void utworz_rachunek() 
	{
		cout << endl << "\t---Tworzenie rachunku---" << endl;
		cout << "Rachunek klienta: "<< endl;
		cout << "  Data wystawienia: " << data_wystawienia << endl;
		cout << "  Data platnosci: " << data_platnosci << endl;
		cout << "  Kwota do zaplaty: " << kwota << endl;

	}


	int zaplac(int czas_pobytu, double& kwota)        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	{  
		cout << endl << "\t---Należna kwota za pobyt---" << endl;
		cout << "Dlugosc pobytu: " << czas_pobytu << endl;
		cout << "Kwota do zaplaty: " << kwota << endl;
		return 0;
	}
};

//class Administrator_systemu{
	//double id_administratora;
	/*void zarzadzaj_permisjami(const id_administratora, string login, string haslo, string permisje) { //dokonczyc koncowka
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
	}	Administrator_systemu(){ //pseudolosowe generowanie id rezerwacji. zakres 1-1000
        srand(time(0));
        id_rezerwacji=rand()%1000+1; 
    }
	int zarzadzaj_kontami(string id_konta_goscia, int wybor) {   /// !!!!!!!!!!!!!!!1
		Administrator_systemu(){ //pseudolosowe generowanie id rezerwacji. zakres 1-1000
        srand(time(0));
        id_konta_goscia=rand()%1000+1; 
    	}
		cout << "Jakie dzialania podjac z kontem " << id_konta_goscia << " ?"<<endl;
		cout << "Reset konta - 1, Zmiana id - 2, Anuluj - 3"<<endl;
		cin >> wybor;
		for(;;){
			if(wybor > 3 || wybor < 1){
				cout<<"Bledna, nieistniejaca opcja!"<<endl;
				break;
			}
			if(wybor == 1){
				id_konta_goscia=0;
			}else if(wybor == 2){
				cout<<"Podaj nowe id." <<endl;
				cin >> id_konta_goscia;
				cout <<"Id konta zaktualizowane!"<<endl;
			}else	
				break;

		}
	}
};*/


/*class I_menadzer_rezerwacji {
public:
	virtual void stworz_rezerwacje(int id_goscia, Rezerwacja rezerwacja) = 0;
	virtual void anuluj_rezerwacje(int id_rezerwacji) = 0;
	virtual void aktualizuj_rezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) = 0;
	virtual vector<Rezerwacja> wyswietl_rezerwacje(int id_goscia) = 0;
	virtual ~I_menadzer_rezerwacji() = default;  //wirtualny destruktor
};
class Menadzer_rezerwacji : public I_menadzer_rezerwacji {	  //klucz: id_goscia, hash-table na wartosci klucz-gosc
	unordered_map<int, vector<Rezerwacja>> rezerwacje-mapa;  //z biblioteki <unordered_map> funckja dzialajaca jak powyzej
public:
	void zrob_rezerwacje(int id_goscia, Rezerwacja rezerwacja) override {} //do nadpisywania i wychwytywania bledow dot klasy bazowej    !!!!!!!
	void anuluj_rezerwacje(int id_rezerwacji) override {}
	void aktualizuj_rezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) override {}
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
*//*
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

class Hotel //klasa wstepnie skonczona - dziala
{ 
public:
	const static string nazwa, adres;
	const static float l_gwiazdek;
	int liczba_pokoi[3];
	int wyswietl_informacje_o_hotelu(const float& l_gwiazdek, const string& nazwa, const string& adres) // metoda dziala
	{  
		cout << "\n\t---Wyswietlanie informacji o hotelu---" << endl;
		cout << "Nazwa: " << nazwa << endl;
		cout << "Adres: " << adres << endl;
		cout << "Liczba gwiazdek: ";
		for(int i = 0; i < l_gwiazdek; i++)
			cout << "*";
		cout << endl;
		return 0;
	}
	int wyswietl_dostepne_pokoje() 
	{
	    string i;
		srand(time(NULL));
		for(int i = 0; i < 3; ++i) 
		{
            liczba_pokoi[i] = rand() % 123 + 1;
        }
		cout << "\t---wyswietlanie dostepnych pokoi---" << endl;
	    cout << "Podaj standard pokoju, gdzie standard(0), studio(1) i premium(2): " << endl;
	    cin >> i;
		int standard = -1;
		if(i == "0" || i == "standard")
   	 	{
        	standard = 0;  // Standard
   	 	}else if(i == "1" || i == "studio")
    	{
        	standard = 1;  // Studio
    	}else if(i == "2" || i == "premium")
    	{
        	standard = 2;  // Premium
    	}else 
   	 	{
        	cout << "Tworzenie rezerwacji zakończone niepowodzeniem!" << endl;
        	return 0;
    	}

		switch(standard)
		{
			case 0:
				if(liczba_pokoi[0]==0)
				{
					cout << "Brak dostepnych pokoi w tym standardzie! " << endl << endl;
					return 0;
				}
				cout << "Liczba dostepnych pokoi 'standard': " << liczba_pokoi[0] << endl << endl;
				return 0;
			case 1:
				if(liczba_pokoi[1]==0)
				{
					cout << "Brak dostepnych pokoi w tym standardzie! " << endl << endl;
					return 0;
				}
				cout << "Liczba dostepnych pokoi 'studio': " << liczba_pokoi[1] << endl << endl;
				return 0;
			case 2:
				if(liczba_pokoi[2]==0)
				{
					cout << "Brak dostepnych pokoi w tym standardzie! " << endl;
					return 0;
				}
				cout << "Liczba dostepnych pokoi 'premium': " << liczba_pokoi[2] << endl << endl;
				return 0;
			default:
				cout << "Podano niepoprawną wartość!" << endl << endl;
	            return 0;
		};
	}
};


int main()
{
	Hotel h;
	h.wyswietl_informacje_o_hotelu(4, "Hotel na potrzeby projektu", "ul. Konieczna 4");
	cout << endl << endl;
	czekaj(1);
	h.wyswietl_dostepne_pokoje();
	czekaj(1);


	Gosc hotel("data/my_sqlite3_hotele_baza.sqlite3");
    vector<string> wyniki = hotel.zobacz_oferty_hoteli("", 3);  // filtr dotyczacy gwiazdek naprawic
    for(const string& wynik : wyniki)
        cout << wynik << endl;
	cout << endl;
	czekaj(1);


	Rezerwacja rez;
	rez.utworz_rezerwacje();
	int czas_pobytu = rez.oblicz_czas_pobytu();
	czekaj(1);
	rez.aktualizuj_rezerwacje();
	czas_pobytu = rez.oblicz_czas_pobytu();
	cout<<endl;
	czekaj(1);
	

	Rachunek rach;
	//int id_rezerwacji = rach.getIdRezerwacji();
	//rach.pobierzDaneZBazy(id_rezerwacji);
	rach.utworz_rachunek();
	czekaj(1);
	double cena_za_dobe = 100.0;  // Ustawiona domyslnie cena za dobę
	double kwota = cena_za_dobe * czas_pobytu;
	rach.zaplac(czas_pobytu, kwota);
	czekaj(1);
	

	return 0;
}