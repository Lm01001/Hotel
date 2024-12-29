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
#include "sqlite3.h"
#include <chrono>
#include <thread>

using namespace std; // kompilacja github/vscode:  g++ -std=c++11 -o hotel_program Projekt_projektowanie_oprogramowania_hotel.cpp -lsqlite3

// Wstrzymanie na programu na podana ilosc sekund  		//ok 
void czekaj(int s) 
{
    this_thread::sleep_for(chrono::seconds(s)); 
}

//  													//dorobic wybor i zapisanie hotelu
class Gosc
{
	string imie, email, nazwisko, dane;
	double nr_tel, id_goscia;
	sqlite3* db;
public:       //sprawdzic czy generowanie id dla goscia potrzebne !!!!!!!!!!!
	//pseudolosowe generowanie id goscia. zakres 1-1000
	Gosc() 
	{ 

        srand(time(0));
        id_goscia = rand() % 15000 + 1; //mamy te dane podawane w rezerwacji 
    }  
	//
	Gosc(const string& db_path)
	{
        if(sqlite3_open(db_path.c_str(), &db))
		{
            cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
            db = nullptr;
			exit(1);
        }
    }
	//
	vector<string> zobacz_oferty_hoteli(const string& panstwo = "", int min_gwiazdek = 0) 
	{
		vector<string> oferty;
		cout << endl << "\t---Wyswietlanie dostepnych hoteli---" << endl;
		if(!db)
		{
            cerr << "Baza danych nie jest otwarta!" << endl;
            return oferty;
        }
        string sql = "SELECT nazwa_hotelu, ilosc_gwiazdek, miasto, panstwo FROM Hotele WHERE 1=1";
        if(!panstwo.empty())
          	sql += " AND panstwo = '" + panstwo + "'";
        if(min_gwiazdek > 0)
            sql += " AND ilosc_gwiazdek >= " + to_string(min_gwiazdek);
        auto callback = [](void* oferty, int argc, char** argv, char** azColName) -> int // Funkcja callback do przetwarzania wyników
		{
            vector<string>* wyniki_vector = static_cast<vector<string>*>(oferty);
            string wynik = "Hotel: " + string(argv[0]) + ", Gwiazdki: " + string(argv[1]) + ", Miasto: " + string(argv[2]) + ", Panstwo: " + string(argv[3]);
            wyniki_vector->push_back(wynik);
            return 0;
        };
		cout << endl;
       	char* error = nullptr;
        if(sqlite3_exec(db, sql.c_str(), callback, &oferty, &error)!=SQLITE_OK)
		{
            cerr<<"Błąd podczas wykonywania zapytania: "<<error<< endl;
            sqlite3_free(error);
        }
		return oferty;
	}
	//destruktor, zamkniecie bazy danych
	~Gosc() 
	{ 
        if(db) 
        	sqlite3_close(db);
    }
};

//														//wstepnie ok
class Rezerwacja
{
	string poczatek_rezerwacji, koniec_rezerwacji, dane;
	double id_rezerwacji;
public:
	// 
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

	// Pseudolosowe generowanie id rezerwacji. zakres 1-1000
	Rezerwacja() 
	{ 
        srand(time(0));
        id_rezerwacji+=rand()%1000+1; 
    }

	//
	tm string_do_tm(const string& data)
    {
        tm t = {};
        istringstream ss(data);
        ss >> get_time(&t, "%d-%m-%Y");
        return t;
    }

	void utworz_rezerwacje()
	{   
		string standard_pokoju, imie_goscia, nazwisko_goscia;
		int i=0;
		sqlite3* db;
    	if (sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 	
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
			string sql = "INSERT INTO Rezerwacje (id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, poczatek_rezerwacji, koniec_rezerwacji, pomoc) VALUES (" + to_string(id_rezerwacji) + ", '" + imie_goscia + "', '" + nazwisko_goscia + "', '" + standard_pokoju + "', '" + poczatek_rezerwacji + "', '" + koniec_rezerwacji + "', CURRENT_TIMESTAMP);";
    	char* err_msg = nullptr;
    	if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK){
        cerr << endl << endl << "\tBłąd podczas zapisu rezerwacji do bazy danych: " << err_msg << endl;
        sqlite3_free(err_msg);
    	}
		string fetch_ostatni= "SELECT * FROM Rezerwacje ORDER BY id_rezerwacji DESC LIMIT 1;";
		sqlite3_stmt* stmt;
		int rc = sqlite3_prepare_v2(db, fetch_ostatni.c_str(), -1, &stmt, 0);
        if (rc != SQLITE_OK) 
        {
            cerr << "Error while preparing SQL query: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) 
        {
            int id = sqlite3_column_int(stmt, 0);
            const char* imie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* nazwisko = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* standard = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* poczatek = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* koniec = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        } 
		sqlite3_finalize(stmt);
    	sqlite3_close(db);
	}

	// Destruktor
	~Rezerwacja() {} 

	// Obliczanie czasu pobytu na bazie podanych dat poczatka i konca
	int oblicz_czas_pobytu()
    {
        tm poczatek = string_do_tm(poczatek_rezerwacji);
        tm koniec = string_do_tm(koniec_rezerwacji);
        time_t t_poczatek = mktime(&poczatek); // Konwersja na czas unixowy - od 1970-01-01
        time_t t_koniec = mktime(&koniec);
        double roznica = difftime(t_koniec, t_poczatek); // Roznica w sekundach
        int dni_pobytu = roznica / (60 * 60 * 24);  // Zamiana sekund na dni
        return dni_pobytu;
    }

	//
	/*void usun_rezerwacje() // exception handling potrzebny, by nie usunac wiecej niz tylko swojej rezerwacji
	{ 
		char i = ' ';
		cout << endl << "\t---Usuwanie rezerwacji---" << endl << "Czy na pewno chcesz usunac swoja rezerwacje? t/n" << endl;
		cin >> i;
		if(tolower(i) == 'n'){
			return;
		}else if(tolower(i) == 't'){
			cout << "Kontynuacja procesu usuwania rezerwacji" << endl;
		}else
			return;
		sqlite3* db;
    	if (sqlite3_open("data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
    	string fetch_ostatni = "SELECT id_rezerwacji FROM Rezerwacje ORDER BY id_rezerwacji DESC LIMIT 1;"; // Znalezienie ostatniej, dodanej rezerwacji
    	sqlite3_stmt* stmt;
    	int rc = sqlite3_prepare_v2(db, fetch_ostatni.c_str(), -1, &stmt, 0);
    	if(rc != SQLITE_OK) 
		{
        	cerr << "Błąd podczas przygotowywania zapytania: " << sqlite3_errmsg(db) << endl;
        	sqlite3_finalize(stmt);
        	sqlite3_close(db);
        	return;
    	}
    	rc = sqlite3_step(stmt);
    	if(rc == SQLITE_ROW) 
		{
        	int id_rezerwacji = sqlite3_column_int(stmt, 0);
        	string sql = "DELETE FROM Rezerwacje WHERE id_rezerwacji = " + to_string(id_rezerwacji) + ";";
        	char* err_msg = nullptr;
        	rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
        	if (rc != SQLITE_OK) {
            	cerr << "Niepowodzenie przy usuwaniu! " << err_msg << endl;
            	sqlite3_free(err_msg);
        	}else{
            	cout << "Rezerwacja " << id_rezerwacji << " usunięta pomyslnie." << endl;
        	}
    	}else 
        cout << "Rezerwacja o takim numerze nie istnieje." << endl << endl;
    	sqlite3_finalize(stmt);
    	sqlite3_close(db);
	}*/

	//aktualizacja rezerwacji, tworzenie rezerwacji na nowo
	void aktualizuj_rezerwacje()
	{
		int a;
		cout << endl << endl << "\t---Aktualizowanie rezerwacji---" << endl;
		cout << "Czy chcesz zaktualizowac calkowicie rezerwacje?  1 - tak, 0 - nie" << endl;
		cin >> a;
		string standard_pokoju, imie_goscia, nazwisko_goscia;
		int i = 0;

			if(a == 1)
			{   	
				cout << "Podaj imie: "; cin>>imie_goscia;
				cout << "Podaj nazwisko: "; cin>>nazwisko_goscia;
				cout << "Wybierz standard pokoju('standard', 'studio' lub 'premium'): "; cin>>standard_pokoju;
				transform(standard_pokoju.begin(), standard_pokoju.end(), standard_pokoju.begin(), ::tolower);
				if(standard_pokoju != "standard" && standard_pokoju != "studio" && standard_pokoju != "premium")
				{
					cout << "Taki standard nie istnieje. Tworzenie rezerwacji zakończone niepowodzeniem!" << endl;
					exit(1);
				}
			}else if(a == 0){
				return;
			}
			else{
				cout<< "Wybrano nieistniejaca opcje.";
				return;
			}

		for(;;) 
		{
        	cout << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin >> poczatek_rezerwacji;
        	if(!poprawna_data(poczatek_rezerwacji)) 
           		cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
         	else 
            	break;
        }
		for(;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR lub DD.MM.RRRR): ";
        	cin >> koniec_rezerwacji;
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
			cout << dane << endl << endl;	
			return;
	}
};

// 														// ok
class Rachunek
{
	double kwota;
	string data_wystawienia, data_platnosci;
	int id_rezerwacji;
public:
	int utworz_rachunek(double& kwota) 
	{
    	sqlite3* db;
		sqlite3_stmt *stmt;
    	if (sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Niepowodzenie przy pobieraniu danych: " << sqlite3_errmsg(db) << endl;
        	return 0;
    	}
    	const char *sql = "SELECT id_rezerwacji, poczatek_rezerwacji, koniec_rezerwacji FROM Rezerwacje ORDER BY pomoc DESC LIMIT 1;"; //ASC LIMIT 1 OFFSET 1   ORDER BY id_rezerwacji DESC LIMIT 1
    	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    	if(rc != SQLITE_OK) 
		{
        	cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return 0;
    	}
    	rc = sqlite3_step(stmt);
    	if(rc == SQLITE_ROW) 
		{
        	int ost = sqlite3_column_int(stmt, 0);  // zapis ostatniego id; indeks 0 dla pierwszej kolumny
        	const char *poczatek_rezerwacji = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        	const char *koniec_rezerwacji = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)); 
        	string poczatek_rez(poczatek_rezerwacji);
        	string koniec_rez(koniec_rezerwacji);
        	cout << endl << "\t---Tworzenie rachunku---" << endl;
        	cout << "Rachunek klienta: "<< endl;
        	cout << "  ID rezerwacji: " << ost << endl << "  Data wystawienia: " << poczatek_rez << endl;
			cout << "  Data platnosci: " << koniec_rez << endl;
			cout << "  Kwota do zaplaty: " << kwota << endl << endl;
    	}else 
        	cerr << "Blad przy pobieraniu danych." << endl;
    	sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie bazy danych
    	sqlite3_close(db);
    	return 1;    
	}

	int zaplac(int czas_pobytu, double& kwota)    // mozna dodac opcje rozne ceny za rozne standardy
	{  
		cout << endl << "\t---Należna kwota za pobyt---" << endl;
		cout << "Dlugosc pobytu: " << czas_pobytu << " dni" << endl;
		cout << "Kwota do zaplaty: " << kwota << endl << endl;
		return 1;
	}
};

//
/*class Administrator_systemu{
	//double id_administratora;
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

//
/*class I_menadzer_rezerwacji 
{
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
};*/

//
/*class Pokoj
{
	int numer;
	bool dostepnosc;
public:
	string standard;
	float cena;
	bool ustaw_status(const int& numer) 
	{

	}
	wyswietl_informacje_pokoju(const int& numer, int id_rezerwacji) 
	{

	}
	pokaz_dostepnosc_standardu(const bool& dostepnosc, int& numer) 
	{

	}
};*/

//														//rejestracja godzin, weryfikacja poprawnosci godzin i !(poczatek>koniec)
class Pracownik
{
protected:
	string imie, nazwisko, rola;
	double id_pracownika;
public:
	// konstruktor, utworzony w celu umozliwienia klasie menadzer_hotelu dziedziczenie elementow protected 
	Pracownik (string& im, string& nazw, string& rol, double& id_prac) : imie(im), nazwisko (nazw), rola(rol){} 
	
	//pseudolosowe generowanie id pracownika. zakres 1-1000
	Pracownik() 
	{ 
        srand(time(0));
        id_pracownika = rand() % 5000 + 1; 
    } 

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

	tm string_do_tm(const string& data)
    {
        tm t = {};
        istringstream ss(data);
        ss >> get_time(&t, "%d-%m-%Y");
        return t;
    }
	
	void aktualizuj_dostepnosc() // ??? atrybuty  // dodawac numer pokoju?? moze baza danych z losowymi danymi pracownika
	{
		string data;
		char usun = '0';
		cout << endl << "\t---Aktualizacja dostepnosci---" << endl;
		cout << "Podaj dzisiejsza date (w formacie: DD-MM-RRRR lub DD.MM.RRRR) w celu sprawdzenia mozliwosc zaktualizowania dostepnosci." << endl; // dodac ilosc standardow jakos wczesniej
		cout << "W przypadku checi usuniecia rezerwacji, wpisz konkretna date jej zakonczenia i wybierz bazujac na jej id. " << endl;
		for(;;) 
		{
			cin >> data;
        	if(!poprawna_data(data))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR lub DD.MM.RRRR" << endl;
        	else 
            	break;
        }
		cout << "---Rezerwacje konczace sie w dniu dzisiejszym---" << endl;
		sqlite3* db;
		sqlite3_stmt *stmt;
    	if (sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Niepowodzenie przy pobieraniu danych: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
    	string sql = "SELECT id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, koniec_rezerwacji FROM Rezerwacje WHERE koniec_rezerwacji = ?;"; 
    	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    	if(rc != SQLITE_OK) 
		{
        	cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(db) << endl;
			sqlite3_close(db);
        	return;
    	}
    	sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_STATIC);
		rc = sqlite3_step(stmt);
    	while(rc == SQLITE_ROW) 
		{
			cout << "| " << "id_rezerwacji:  " << sqlite3_column_text(stmt, 0) << " | " << "imie_goscia:  " << sqlite3_column_text(stmt, 1) << " | "
        	<< "nazwisko_goscia:  " << sqlite3_column_text(stmt, 2) << " | " << "standard_pokoju:  " << sqlite3_column_text(stmt, 3) << " | " << sqlite3_column_text(stmt, 4) << endl;
			rc = sqlite3_step(stmt);
    	}
		cout << "Podaj id rezerwacji, ktora chcesz usunac. " << endl;
		cin >> usun;
		if(!isdigit((char)(usun)))
			return;
		rc = sqlite3_step(stmt);
    	if (rc == SQLITE_ROW) 
		{
        	data = sqlite3_column_int(stmt, 0);
        	string sql = "DELETE FROM Rezerwacje WHERE id_rezerwacji = " + to_string(usun) + ";"; 
        	char* err_msg = nullptr;
			czekaj(1);
       		rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg);
        	if(rc != SQLITE_OK) 
			{
            	cerr << "Niepowodzenie przy usuwaniu! " << err_msg << endl;
            	sqlite3_free(err_msg);
        	}else 
			{
            	cout << "Rezerwacja " << data << " usunięta pomyslnie." << endl << endl; //!!!!!!!!!!!!!!
        	}
    	}else 
        	cout << "Rezerwacja o takim numerze nie istnieje." << endl << endl;
    	sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie bazy danych
    	sqlite3_close(db);
    	return; 
	}

	void zarejstruj_godziny_pracy(double id_pracownika) //!!!!!zmienic id_pracownika i nie dawac jako argument
	{
		sqlite3* db;
		sqlite3_stmt* stmt;
		string poczatek_pracy, koniec_pracy;
		cout << endl << "\t---Rejestracja godzin pracy---" << endl;
		cout << "Podaj godzine poczatku pracy (w formacie hh:mm): " << endl;
		cin >> poczatek_pracy;
		cout << "Podaj godzine konca pracy (w formacie hh:mm): " << endl;
		cin >> koniec_pracy;
		if(sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Błąd przy otwieraniu bazy danych: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
		string sql = "INSERT INTO Rejestrowanie (id_pracownika, poczatek_pracy, koniec_pracy, dzien_tygodnia) "
                      "VALUES (?, ?, ?, CASE strftime('%w', ?) "
					  "WHEN '0' THEN 'Sobota' "
                      "WHEN '1' THEN 'Niedziela' "
                      "WHEN '2' THEN 'Poniedziałek' "
                      "WHEN '3' THEN 'Wtorek' "
                      "WHEN '4' THEN 'Środa' "
                      "WHEN '5' THEN 'Czwartek' "
                      "WHEN '6' THEN 'Piątek' "
                      "END);";
		if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
		{
        	cerr << "Błąd przygotowania zapytania: " << sqlite3_errmsg(db) << endl;
			sqlite3_finalize(stmt);
        	sqlite3_close(db);
        	return;
    	}
		sqlite3_bind_int(stmt, 1, id_pracownika);
    	sqlite3_bind_text(stmt, 2, poczatek_pracy.c_str(), -1, SQLITE_STATIC);
    	sqlite3_bind_text(stmt, 3, koniec_pracy.c_str(), -1, SQLITE_STATIC);
    	sqlite3_bind_text(stmt, 4, poczatek_pracy.c_str(), -1, SQLITE_STATIC);
    	if(sqlite3_step(stmt) != SQLITE_DONE) 
		{
        	cerr << "Błąd wykonywania zapytania: " << sqlite3_errmsg(db) << endl;
    	}else 
		{
        	cout << "Dane dodane pomyślnie!" << endl;
    	}
		sqlite3_finalize(stmt);
		sql = "SELECT id_pracownika, poczatek_pracy, koniec_pracy, dzien_tygodnia "
              "FROM Rejestrowanie;";
    	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    	{
        	cerr << "Błąd przygotowania zapytania SELECT: " << sqlite3_errmsg(db) << endl;
        	sqlite3_close(db);
        	return;
    	}
		cout << "Tabela zarejstrowanych godzin pracy: " << endl;
		int kolumny = sqlite3_column_count(stmt); // Liczba kolumn w tabeli
    	while (sqlite3_step(stmt) == SQLITE_ROW)
    	{
        	for (int i = 0; i < kolumny; i++)
        	{
            	const char* wartosc = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            	cout << (wartosc ? wartosc : "NULL") << " | ";
        	}
        	cout << endl;
    	}
		sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie db
    	sqlite3_close(db);
		cout << endl << endl;
	}
};

//
/*class Menadzer_hotelu : public Pracownik
{
	
	int weryfikuj_rezerwacje(const int& id_rezerwacji, string status_rezerwacji) 
	{

	}
public:
	Menadzer_hotelu(string im, string nazw, string rol, double id_prac) : Pracownik(im, nazw, rol, id_prac) {}

	void utworz_raport(double& id_pracownika, double& poczatek_pracy, double& koniec_pracy) 
	{

	}

	void zarzadzaj_personelem(double& id_pracownika, string aktualne_zadanie) 
	{

	}
};
*/

//
/*class System_hotelowy
{
	double wersja_oprogramowania_sys;  //czy cos ponizej??

	void konfiguruj_ustawienia_systemowe() 
	{

	}
protected:
	void sprawdz_logi_systemu(bool awaria, double czas_zdarzenia, double id_uzytkownika, string okolicznosci_zdarzenia,
		 string poziom_logu) //idk czy trzeba???
		{ 
		if (!awaria)
		{
			cout << "Brak awarii. Konczenie procesu." << endl;
			return;
		}
		else
		{
			cout << "Wystapienie awarii !!!" <<endl;
			cout << "czas: " <<czas_zdarzenia<<" " << "okolicznosci zdarzenia" << endl << "uzytkownik: " << id_uzytkownika <<
				" poziom logu: " << poziom_logu << endl;
			return;
		}
	}

	int zamelduj_goscia(double id_goscia, int id_rezerwacji) // mozna by zrobic unordered map i dac 
	{ 
		//unordered_map<id_goscia, id_rezerwacji> gosc-rezerwacja;
		unordered_map<double, int> gosc_rezerwacja; //gosc jako klucz id rezerwacji sprawdzic  !!!!!!
		if (!id_goscia || !id_rezerwacji)  
			return 0;
		else
			return 1;
			/*przypisanie wartosci:
			gosc_rezerwacja[id_goscia] = id_rezerwacji;
			przjescie calej for(auto& x : gosc_rezerwacja) przjedzie od konca
			x.first << x.second;
			lub tak lepiej albo tu db i sprawdz logi tam jakos wygenerowac
			for
	}
	int wymelduj_goscia(double id_goscia, double dodatkowe_oplaty) 
	{
		if(dodatkowe_oplaty != 0)
		{
			cout << "Prosba o uregulowanie naleznej kwoty. " << endl;
			//mozna sprobowac w rachunku klasie dac static i uzyc metody
		}
		return 1;
	}
};*/

//
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
		cout << endl << endl << endl;
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
{	//poprawic estetyke tych komentarzy
	// Klasa Hotel, Metody: wyswietl_informacje_o_hotelu(), wyswietl_dostepne_pokoje()
	Hotel h; 
	h.wyswietl_informacje_o_hotelu(4, "Hotel na potrzeby projektu", "ul. Konieczna 4");
	czekaj(1);
	h.wyswietl_dostepne_pokoje();
	czekaj(1);


	Gosc hotel("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3");
    vector<string> wyniki = hotel.zobacz_oferty_hoteli("", 3);  // filtr dotyczacy gwiazdek naprawic
    for(const string& wynik : wyniki)
        cout << wynik << endl;
	czekaj(1);


	Rezerwacja rez;
	rez.utworz_rezerwacje(); //e.h.  poprawnosc daty ograniczenia etc
	int czas_pobytu = rez.oblicz_czas_pobytu();
	czekaj(1);
	rez.aktualizuj_rezerwacje();
	czas_pobytu = rez.oblicz_czas_pobytu();
	czekaj(1);
	

	Rachunek rach;
	double cena_za_dobe = 100.0;  // Ustawiona domyslnie cena za dobę
	double kwota = cena_za_dobe * czas_pobytu;
	rach.utworz_rachunek(kwota);
	czekaj(1);
	rach.zaplac(czas_pobytu, kwota); // nie uwzglednia miesiecy bo 30.01-20.02 to  -10dni
	czekaj(1);
	

	string imie = "nie", nazwisko = "tak", stanowisko = "recepcjonista";
	double id_pracownika =123456;
	Pracownik p(imie, nazwisko, stanowisko, id_pracownika);
	p.aktualizuj_dostepnosc();
	czekaj(1);
	p.zarejstruj_godziny_pracy(id_pracownika);
	czekaj(1);

	return 1;
}