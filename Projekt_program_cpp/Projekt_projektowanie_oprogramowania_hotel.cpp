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
#include <cstring>

using namespace std; // kompilacja github/vscode(wsl):  g++ -std=c++11 -o hotel_program Projekt_projektowanie_oprogramowania_hotel.cpp -lsqlite3
//stworzyc nowy terminal przed pokazaniem kodu!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Wstrzymanie na programu na podana ilosc sekund  		//ok 
void czekaj(int s) 
{
    this_thread::sleep_for(chrono::seconds(s)); 
}


//  													//[dorobic wybor i zapisanie hotelu - skonczone]//sprawdzic czy generowanie id dla goscia potrzebne !!!!!!!!!!!
class Gosc
{
	string imie, email, nazwisko, dane;
	int id_goscia;
	sqlite3* db;
public:     
	// Konstruktor domyslny
	Gosc() 
	{ 
		// Pseudolosowe generowanie id goscia. zakres 1-1000
		srand(time(0));
        id_goscia = rand() % 15000 + 1;
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
	vector<string> zobacz_oferty_hoteli() 
	{
		// Obiekt klasy Gosc
		Gosc gosc;
		vector<string> oferty;
		string sql1;
		int wybor, min_gwiazdek = 0;
		char filtry;
		cout << endl << "\t---Wyswietlanie dostepnych hoteli---" << endl << endl;
		
		if(!db)
		{
            cerr << "Baza danych nie jest otwarta!" << endl;
            return {}; // dla bledu zwrot pustego wektora
        }

        
		Poczatek:
			string panstwo = "", miasto = "", opcja = "", i = "", temp = "";
			string sql = "SELECT nazwa_hotelu, ilosc_gwiazdek, miasto, panstwo, adres FROM Hotele WHERE 1=1";
			cout << "W celu wlaczenia filtrowania wpisz 1." << endl;
			cin >> i;
			czekaj(1);
        	if(i == "1")
			{
				cout << endl << "Filtrowanie ofert, wybierz jedna z nastepujacych opcji wpisujac przypisana jej cyfre: " << endl;
				cout << "1. Miasto" << endl << "2. Panstwo" << endl << "3. Minimalna liczba gwiazdek" << endl << endl << "Wybierz opcje: " << endl;
				for(;;)
				{	
					cin >> opcja;
					filtry=opcja[0];
					if((filtry!='1' && filtry!='2' && filtry!='3') ||  !isdigit(filtry)) 
					{
						cout << "Wybrano nieistniejaca opcje." << endl;
						break;
					}
            
					switch(filtry)
					{
						case '1':
							cout << "Podaj nazwe miasta: " << endl;
							cin >> miasto;
							break;
						case '2':
							cout << "Podaj nazwe panstwa: " << endl;
							cin >> panstwo;
							break;
						case '3':
							cout << "Podaj liczbe gwiazdek: " << endl;
							cin >> min_gwiazdek; 
							temp = string(min_gwiazdek, '*');
							break;
						default:
							break;	
					};
					cout << "Czy chcesz kontynuowac filtrowanie? Jesli tak, wpisz 1." << endl;; 
					cin >> i;
					if(i == "1")
					{
						cout << "Wybierz kolejna opcje: " << endl;
						continue;
					}else
						break;	
				}	
				if(!miasto.empty())
			{
				transform(miasto.begin(), miasto.end(), miasto.begin(), ::tolower);
				miasto[0] = ::toupper(miasto[0]);
          		sql += " AND miasto = '" + miasto + "'";
			}
				if(!panstwo.empty())
			{
				transform(panstwo.begin(), panstwo.end(), panstwo.begin(), ::tolower);
				panstwo[0] = ::toupper(panstwo[0]);
          		sql += " AND panstwo = '" + panstwo + "'";
			}
        		if(min_gwiazdek > 0)
            		sql += " AND ilosc_gwiazdek >= '" + temp + "';";
			}

			// Funkcja callback do przetwarzania wyników
        	auto callback = [](void* oferty, int argc, char** argv, char** azColName) -> int 
			{
        	    vector<string>* wyniki_vector = static_cast<vector<string>*>(oferty);
        	    string wynik = "Hotel: " + string(argv[0]) + ", Gwiazdki: " + string(argv[1]) + ", Miasto: " + string(argv[2]) + ", Panstwo: " + string(argv[3]) + ", Adres: " + string(argv[4]);
        	    wyniki_vector->push_back(wynik);
				return 0;
        	};

			cout << endl;
       		char* error = nullptr;
        	if(sqlite3_exec(db, sql.c_str(), callback, &oferty, &error)!=SQLITE_OK)
			{
        	    cerr<<"Błąd podczas wykonywania zapytania: "<<error<< endl;
        	    sqlite3_free(error);
				return {}; // dla bledu zwrot pustego wektora
        	}

			for(size_t i = 0; i < oferty.size(); ++i) 
			{
        		cout << i + 1 << ". " << oferty[i] << endl;
    		}

    		if(oferty.empty()) 
			{	
				cout << "Brak ofert spełniających kryteria." << endl;
				goto Poczatek;
        		//return {};
    		}

    	cout << "\n\tWybierz numer oferty, która Cie interesuje: ";
    	Wybor:
		cin >> wybor;
    	if(wybor < 1 || wybor > oferty.size()) 
		{
    	    cerr << "Nieprawidłowy wybór." << endl;
			cout << "Jesli chcesz wybrac ponownie, podaj 1 , a nastepnie podaj numer oferty."<< endl;
			cin >> wybor;
			if(wybor == 1)
				goto Wybor;
			else
        		return {};
		}

		size_t start = oferty[wybor - 1].find(",") + 12;  
		size_t koniec = oferty[wybor - 1].find(",", start); 
		size_t ostatnia_kol = oferty[wybor - 1].rfind(',') + 7;
		string nazwa_hotelu = oferty[wybor - 1].substr(7, oferty[wybor - 1].find(",") - 7);
		string adres = oferty[wybor - 1].substr(ostatnia_kol + 1);       
		string ilosc_gwiazdek = oferty[wybor - 1].substr(start, koniec - start);
		
		sql1 = "INSERT INTO Informacje_hotel (nazwa_hotelu, standard_pokoju, cena_doba, adres, liczba_gwiazdek, id_goscia) VALUES ('" + nazwa_hotelu + "' , NULL ,  NULL,'" + adres + "', '" + ilosc_gwiazdek + "', '" + to_string(gosc.id_goscia) + "');";

    	if(sqlite3_exec(db, sql1.c_str(), nullptr, nullptr, &error) != SQLITE_OK) 
		{
        	cerr << "Błąd podczas dodawania oferty: " << error << endl << endl;
        	sqlite3_free(error);
			return {};
    	}
		else{
        	cout << "Oferta zostala pomyslnie zapisana." << endl << endl;
		}
		return {};
	}

	// Destruktor, zamkniecie bazy danych
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
	int id_rezerwacji;

public:
	// 
	bool poprawna_data(const string& data)
	{
    	if(data.size() != 10) return false;
    	if(data[2] != '-' || data[5] != '-') 
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


		const char* sql2 = "SELECT standard_pokoju FROM Informacje_hotel";
		sqlite3_stmt* stmt2;
		if(sqlite3_prepare_v2(db, sql2, -1, &stmt2, 0) != SQLITE_OK) 
		{
        	cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
		if(sqlite3_step(stmt2) == SQLITE_ROW) 
		{
        	const char *standard = reinterpret_cast<const char *>(sqlite3_column_text(stmt2, 0));
			standard_pokoju = string(standard);
		}else 
        	cerr << "Blad przy pobieraniu danych." << endl;
		sqlite3_finalize(stmt2);
		
		for(;;) 
		{
        	cout << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR): ";
        	cin>>poczatek_rezerwacji;
        	if(!poprawna_data(poczatek_rezerwacji)) 
           		cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR" << endl;
         	else 
            	break;
        }
		for(;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR): ";
        	cin>>koniec_rezerwacji;
        	if(!poprawna_data(koniec_rezerwacji))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR" << endl;
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

		string sql = "INSERT INTO Rezerwacje (id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, poczatek_rezerwacji, koniec_rezerwacji, pomoc) VALUES ("+ to_string(id_rezerwacji) + ", '" + imie_goscia + "', '" + nazwisko_goscia + "', '" + standard_pokoju + "', '" + poczatek_rezerwacji + "', '" + koniec_rezerwacji + "', CURRENT_TIMESTAMP);";
    	char* err_msg = nullptr;
    	if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK)
		{
        	cerr << endl << endl << "\tBlad podczas zapisu rezerwacji do bazy danych: " << err_msg << endl;
        	sqlite3_free(err_msg);
    	}
		string fetch_ostatni= "SELECT * FROM Rezerwacje ORDER BY id_rezerwacji DESC LIMIT 1;";
		sqlite3_stmt* stmt;
		int rc = sqlite3_prepare_v2(db, fetch_ostatni.c_str(), -1, &stmt, 0);
        if (rc != SQLITE_OK) 
        {
            cerr << "Problem z przygotowywaniem sql query: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return;
        }
        if (sqlite3_step(stmt) == SQLITE_ROW) 
        {
            int id = sqlite3_column_int(stmt, 0);
            const char* imie = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* nazwisko = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* standard = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* poczatek = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            const char* koniec = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        }else 
        	cerr << "Blad przy pobieraniu danych." << endl;
		sqlite3_finalize(stmt);
    	sqlite3_close(db);
	}

	// Destruktor
	~Rezerwacja() {} 

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
        	cout << "Podaj poczatek rezerwacji (w formacie: DD-MM-RRRR): ";
        	cin >> poczatek_rezerwacji;
        	if(!poprawna_data(poczatek_rezerwacji)) 
           		cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR" << endl;
         	else 
            	break;
        }
		for(;;) 
		{
        	cout << "Podaj koniec rezerwacji (w formacie: DD-MM-RRRR): ";
        	cin >> koniec_rezerwacji;
        	if(!poprawna_data(koniec_rezerwacji))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR" << endl;
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
	int czas_pobytu;
	double kwota;
	string data_wystawienia, data_platnosci;
	int id_rezerwacji;
public:
	//
	tm string_to_tm(const string& data)
	{
    	tm t = {};
    	istringstream ss(data);
    	ss >> get_time(&t, "%d-%m-%Y");
    	return t;
	}	

	// Obliczanie czasu pobytu na bazie podanych dat poczatka i konca
	int oblicz_czas_pobytu(const string& poczatek_tm, const string& koniec_tm)   
	{
	    tm poczatek = string_to_tm(poczatek_tm);
		tm koniec = string_to_tm(koniec_tm);
	    time_t t_poczatek = mktime(&poczatek); // Konwersja na czas unixowy - od 1970-01-01
	    time_t t_koniec = mktime(&koniec);
	    double roznica = difftime(t_koniec, t_poczatek); // Roznica w sekundach
	    int dni_pobytu = roznica / (60 * 60 * 24);  // Zamiana sekund na dni
	    return dni_pobytu;
	}

	//
	int utworz_rachunek() 
	{
		double kwota_dbl = 0.0;
		double& kwota = kwota_dbl;
		int pobyt = 0;
		int& czas_pobytu = pobyt;
    	sqlite3* db;
		sqlite3_stmt *stmt;
    	if (sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Niepowodzenie przy pobieraniu danych: " << sqlite3_errmsg(db) << endl;
        	return 0;
    	}
    	const char *sql = "SELECT id_rezerwacji, poczatek_rezerwacji, koniec_rezerwacji, standard_pokoju FROM Rezerwacje ORDER BY pomoc DESC LIMIT 1;"; 
    	if(sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) 
		{
        	cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return 0;
    	}

    	if(sqlite3_step(stmt) == SQLITE_ROW) 
		{
        	int ost = sqlite3_column_int(stmt, 0);  // zapis ostatniego id; indeks 0 dla pierwszej kolumny
        	const char *poczatek_rezerwacji = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        	const char *koniec_rezerwacji = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)); 
			const char *standard_pokoju = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
			if(strcmp(standard_pokoju, "standard"))
			{
				kwota = 120;
			}
			else if(strcmp(standard_pokoju, "studio")){
				kwota = 155;
			}
			else if(strcmp(standard_pokoju, "premium")){
				kwota= 200;
			}
        	string poczatek_rezerwacji_st(poczatek_rezerwacji);
        	string koniec_rezerwacji_st(koniec_rezerwacji);
			czas_pobytu = oblicz_czas_pobytu(poczatek_rezerwacji, koniec_rezerwacji);
			kwota = kwota * czas_pobytu;
        	cout << endl << endl << "\t---Tworzenie rachunku---" << endl;
        	cout << "Rachunek klienta: "<< endl;
        	cout << "  ID rezerwacji: " << ost << endl << "  Data wystawienia: " << poczatek_rezerwacji_st << endl;
			cout << "  Data platnosci: " << koniec_rezerwacji_st << endl;
			cout << "  Kwota do zaplaty: " << kwota << endl << endl;
    	}else 
        	cerr << "Blad przy pobieraniu danych." << endl;
    	sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie bazy danych
    	sqlite3_close(db);
    	return 0;  
	}

	//
	int zaplac()    // mozna dodac opcje rozne ceny za rozne standardy
	{  
		int pobyt = 0;
		int& czas_pobytu = pobyt;
		double kwota_dbl = 0.0;
		double& kwota = kwota_dbl;
		sqlite3* db;
		sqlite3_stmt* stmt;
    	if(sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 	
		{
        	cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
        	return 0;
    	}
		string sql = "SELECT nazwa_hotelu, liczba_gwiazdek, adres, standard_pokoju FROM Informacje_hotel ORDER BY ROWID DESC LIMIT 1";
    	if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
    	{
     		cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return -1;
    	}

		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			const char* nazwa_hotelu = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			//int liczba_gwiazdek = (sqlite3_column_int(stmt, 1));
			const char* liczba_gwiazdek = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        	const char* adres = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
			const char* standard_pokoju = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
	
			if(strcmp(standard_pokoju, "standard") == 0)
			{
				kwota = 120;
			}
			else if(strcmp(standard_pokoju, "studio") == 0){
				kwota = 155;
			}
			else if(strcmp(standard_pokoju, "premium") == 0){
				kwota= 200;
			}

			//czas_pobytu = oblicz_czas_pobytu(poczatek_rezerwacji, koniec_rezerwacji);
			//kwota = kwota * czas_pobytu;
			cout << endl << "\t---Należna kwota za pobyt---" << endl;
			cout << "Hotel: " << nazwa_hotelu << " " << liczba_gwiazdek << endl;			
			cout << "Adres: " << adres << endl;
			cout << "Dlugosc pobytu: " << czas_pobytu << " dni" << endl;
			cout << "Standard pokoju: " << standard_pokoju << endl;
			cout << "Kwota do zaplaty: " << kwota << endl << endl;
		}
		else{
			cerr << "Nie znaleziono danych w tabeli." << endl;
		}

		sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie db
    	sqlite3_close(db);
		return 0;
	}
};

//
/*class Administrator_systemu
{
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


//    													//powinno byc wstepnie ok, sprawdzic w praktyce
class I_menadzer_rezerwacji 
{
public:
	//metody wirtualne, (=0) deklaracja metody jako czysto wirtualnej zapewnienie polimorfizmu;
	//mozliwe odwolywanie sie do nich - faktyczna implementacja przez klase pochodna
	virtual void stworz_rezerwacje(int id_goscia, Rezerwacja rezerwacja) = 0;
	virtual void anuluj_rezerwacje(int id_rezerwacji) = 0;
	virtual void aktualizuj_rezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) = 0;
	virtual vector<Rezerwacja> wyswietl_rezerwacje(int id_goscia) = 0;
	
	//wirtualny destruktor
	virtual ~I_menadzer_rezerwacji() = default;  
};


//														//powinno byc wstepnie ok, sprawdzic w praktyce ?????
/*class Menadzer_rezerwacji : public I_menadzer_rezerwacji 
{
	//klucz: id_goscia, hash-table na wartosci klucz-gosc; z biblioteki <unordered_map>
	//funckja dzialajaca jak powyzej
	unordered_map<int, vector<Rezerwacja>> rezerwacje_mapa;  
public:
	//do nadpisywania i wychwytywania bledow dot. klasy bazowej 
	void stworz_rezerwacje(int id_goscia, Rezerwacja rezerwacja) override {} 
	void anuluj_rezerwacje(int id_rezerwacji) override {}
	void aktualizuj_rezerwacje(int id_rezerwacji, Rezerwacja nowa_rezerwacja) override {}
	vector<Rezerwacja> wyswietl_rezerwacje(int id_goscia) override {}
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
	int id_pracownik;
	int& id_pracownika = id_pracownik;
public:
	// Konstruktor, utworzony w celu umozliwienia klasie menadzer_hotelu dziedziczenie elementow protected 
	Pracownik (string& im, string& nazw, string& rol, double& id_prac) : imie(im), nazwisko (nazw), rola(rol){} 
	
	// Pseudolosowe generowanie id pracownika. zakres 1-1000
	Pracownik() 
	{ 
        srand(time(0));
        id_pracownika = rand() % 5000 + 1; 
    } 

	//
	bool poprawna_data(const string& data)
	{
    	if(data.size() != 10) return false;
    	if(data[2] != '-' || data[5] != '-') 
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

	//
	tm string_do_tm(const string& data)
    {
        tm t = {};
        istringstream ss(data);
        ss >> get_time(&t, "%d-%m-%Y");
        return t;
    }
	
	// Zrobic konwersje dat do jednego ujednoliconego typu
	void aktualizuj_dostepnosc() // ??? atrybuty  // dodawac numer pokoju?? moze baza danych z losowymi danymi pracownika
	{
		string data, usun = "";
		cout << endl << "\t---Aktualizacja dostepnosci---" << endl;
		cout << "Podaj dzisiejsza date (w formacie: DD-MM-RRRR) w celu sprawdzenia mozliwosc zaktualizowania dostepnosci." << endl; // dodac ilosc standardow jakos wczesniej
		cout << "W przypadku checi usuniecia rezerwacji, wpisz konkretna date jej zakonczenia i wybierz bazujac, przypisana do jej id. " << endl;
		for(;;) 
		{
			cin >> data;
        	if(!poprawna_data(data))
            	cout << endl << "Niepoprawny format! Podaj datę ponownie. W formacie DD-MM-RRRR" << endl;
        	else 
            	break;
        }
		cout << "\t---Rezerwacje konczace sie w dniu dzisiejszym---" << endl;
		sqlite3* db;
		sqlite3_stmt *stmt;
    	if (sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Niepowodzenie przy pobieraniu danych: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
    	string sql = "SELECT id_rezerwacji, imie_goscia, nazwisko_goscia, standard_pokoju, koniec_rezerwacji FROM Rezerwacje WHERE koniec_rezerwacji = ?;"; 
    	if(sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) != SQLITE_OK) 
		{
        	cerr << "Nie udało się przygotować zapytania: " << sqlite3_errmsg(db) << endl;
			sqlite3_close(db);
        	return;
    	}
    	sqlite3_bind_text(stmt, 1, data.c_str(), -1, SQLITE_STATIC);
		if(data.empty())
		{
			cout << "Brak rezerwacji spelniajacych warunek! " << endl;
			return;
		}
    	while(sqlite3_step(stmt) == SQLITE_ROW) 
		{
			cout << "| " << "id_rezerwacji:  " << sqlite3_column_text(stmt, 0) << " | " << "imie_goscia:  " << sqlite3_column_text(stmt, 1) << " | "
        	<< "nazwisko_goscia:  " << sqlite3_column_text(stmt, 2) << " | " << "standard_pokoju:  " << sqlite3_column_text(stmt, 3) << " | " << sqlite3_column_text(stmt, 4) << endl;
    	}
		cout << "Podaj id rezerwacji, ktora chcesz usunac. " << endl;
		cin >> usun;
		if(usun.empty() || !all_of(usun.begin(), usun.end(), ::isdigit))
			return;
    	if (sqlite3_step(stmt) == SQLITE_ROW) 
		{
        	data = sqlite3_column_int(stmt, 0);
        	string sql = "DELETE FROM Rezerwacje WHERE id_rezerwacji = " + usun + ";"; 
        	char* err_msg = nullptr;
			czekaj(1);
        	if(sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) 
			{
            	cerr << "Niepowodzenie przy usuwaniu! " << err_msg << endl;
            	sqlite3_free(err_msg);
        	}
			/*else if(sqlite3_column_text(stmt, 4) != usun)
			{ 
				cout << "Rezerwacja o tym numerze nie spelnia warunku poczatkowego! " << endl;
			}*/else{
            	cout << "Rezerwacja " << usun << " usunięta pomyslnie." << endl << endl; //!!!!!!!!!!!!!!
        	}
    	}else 
        	cout << "Rezerwacja o takim numerze nie istnieje." << endl << endl;
    	sqlite3_finalize(stmt); // zwalnianie zasobow, zamykanie bazy danych
    	sqlite3_close(db);
    	return; 
	}

	// 
	bool poprawna_godzina(const string& data)
	{
    	if(data.size() != 5) 
			return false;
    	if(data[2] != ':') 
        	return false;
    	for(int i = 0; i < data.size(); ++i) 
		{
       		if (i == 2 ) 
				continue;  // pominiecie "oddzielnika"
        	if (!isdigit(data[i])) 
				return false;
    	}
    	return true;
	}

	//
	void zarejstruj_godziny_pracy() //!!!!!zmienic id_pracownika i nie dawac jako argument
	{	
		Pracownik pracownik_id;
		sqlite3* db;
		sqlite3_stmt* stmt;
		string poczatek_pracy, koniec_pracy;

		cout << endl << "\t---Rejestracja godzin pracy---" << endl;
		for(;;) 
		{
			cout << "Podaj godzine poczatku pracy (w formacie hh:mm): " << endl;
			cin >> poczatek_pracy;
			if(!poprawna_godzina(poczatek_pracy))
				cout << endl << "Niepoprawny format! Podaj godzine ponownie. W formacie HH:MM" << endl;
        	else 
            	break;
		}
		// \/ pobrac z rezerwacje aktualna godzine i przypisac
		//cout << "Podaj godzine konca pracy (w formacie hh:mm): " << endl;
		//cin >> koniec_pracy;
		if(sqlite3_open("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3", &db) != SQLITE_OK) 
		{
        	cerr << "Błąd przy otwieraniu bazy danych: " << sqlite3_errmsg(db) << endl;
        	return;
    	}
		
		string sql = "INSERT INTO Rejestrowanie (id_pracownika, poczatek_pracy, koniec_pracy, dzien_tygodnia) "
                      "VALUES ('" + to_string(pracownik_id.id_pracownika) + "','" + poczatek_pracy + "', strftime('%H:%M', datetime('now', '-576 minutes')), CASE strftime('%w', 'now') "
					  "WHEN '0' THEN 'Sroda' "
                      "WHEN '1' THEN 'Czwartek' "
                      "WHEN '2' THEN 'Piatek' "
                      "WHEN '3' THEN 'Sobota' "
                      "WHEN '4' THEN 'Niedziela' "
                      "WHEN '5' THEN 'Poniedzialek' "
                      "WHEN '6' THEN 'Wtorek' "
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
		cout << endl << "Tabela zarejstrowanych godzin pracy: " << endl;
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

//														//klasa wstepnie skonczona - dziala
class Hotel 
{ 
	sqlite3* db;
public:
	const static string nazwa, adres;
	const static float l_gwiazdek;
	int liczba_pokoi[3];

	// Konstruktor domyslny
	Hotel() : db(nullptr)
	{
		// Do inicjalizacji gen. liczb pseudolosowych
		srand(time(NULL)); 
	}

	// Otwieranie bazy danych
	Hotel(const string& db_path)
	{
        if(sqlite3_open(db_path.c_str(), &db))
		{
            cerr << "Nie można otworzyć bazy danych: " << sqlite3_errmsg(db) << endl;
            db = nullptr;
			exit(1);
        }
    }

	//
	int wyswietl_informacje_o_hotelu() // metoda dziala
	{  
    	string sql = "SELECT nazwa_hotelu, adres, liczba_gwiazdek FROM Informacje_hotel ORDER BY ROWID DESC LIMIT 1;"; 
    	sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
    	{
     		cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return -1;
    	}

		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			const char* nazwa_hotelu = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        	const char* adres = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			const char* liczba_gwiazdek = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
			cout << "\n\t---Wyswietlanie informacji o hotelu---" << endl;
			cout << "Nazwa: " << nazwa_hotelu << endl;
			cout << "Adres: " << adres << endl;
			cout << "Liczba gwiazdek: " << liczba_gwiazdek << endl << endl;
		}
		else{
			cerr << "Nie znaleziono danych w tabeli." << endl;
		}
		sqlite3_finalize(stmt);
		return 0;
	}
	
	//
	int wyswietl_dostepne_pokoje() 
	{
		if(!db)
		{
            cerr << "Baza danych nie jest otwarta!" << endl;
            return 0;
        }
		
	    string i;
		int cena_doba = 0;
		const char* standard_pokoju = nullptr;
		string sql = "SELECT standard_pokoju, cena_doba FROM Informacje_hotel;"; 
		sqlite3_stmt* stmt;
		if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) 
    	{
     		cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
        	return -1;
    	}

		srand(time(NULL));
		// Petla przypisuje pseudolosowe liczby do kazdego ze standardow pokoju
		for(int i = 0; i < 3; ++i) 
		{
            liczba_pokoi[i] = rand() % 123 + 1;
        }

		if(sqlite3_step(stmt) == SQLITE_ROW)
		{
			standard_pokoju = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
			cena_doba = sqlite3_column_int(stmt, 1);
		}
		sqlite3_finalize(stmt);

		cout << endl << "\t---Wyswietlanie dostepnych pokoi---" << endl;
	    cout << "Sprawdzenie dostepnosci jest rownoznaczne z wyborem standardu chyba, ze nie ma dostepnych pokoi."
			 << endl <<"Podaj standard pokoju, gdzie standard(0), studio(1) i premium(2): " << endl; 
		for(;;)
		{	
			cin >> i;
			int standard = -1;
			cena_doba = 0;
			if(i == "0" || i == "standard"){
        		standard = 0;  // Standard - wybrany standard pokoju
				cena_doba = 120;
			}
			else if(i == "1" || i == "studio"){
        		standard = 1;  // Studio - wybrany standard pokoju
				cena_doba = 155;
			}
			else if(i == "2" || i == "premium"){
        		standard = 2;  // Premium - wybrany standard pokoju
				cena_doba = 200;
			}

			if(liczba_pokoi[0] == 0 && liczba_pokoi[1] == 0 && liczba_pokoi[2] == 0)
			{
				cout << "Niestety, brak jakichkolwiek wolnych pokoi w hotelu. " << endl 
					 << "Sprobuj ponownie wybierajac inny hotel.";
				exit(0);
			}

			string nazwa_standard = "";
			switch(standard)
			{
				case 0:
					if(liczba_pokoi[0]==0)
					{
						cout << "Brak dostepnych pokoi w tym standardzie! " << endl;
						cout << "Wybierz inna opcje." << endl;
						continue;
					}
					cout << "Liczba dostepnych pokoi 'standard': " << liczba_pokoi[0] << endl << endl;
					nazwa_standard = "standard";
					break;
				case 1:
					if(liczba_pokoi[1]==0)
					{
						cout << "Brak dostepnych pokoi w tym standardzie! " << endl << endl;
						cout << "Wybierz inna opcje." << endl;
						continue;
					}
					cout << "Liczba dostepnych pokoi 'studio': " << liczba_pokoi[1] << endl << endl;
					nazwa_standard = "studio";
					break;
				case 2:
					if(liczba_pokoi[2]==0)
					{
						cout << "Brak dostepnych pokoi w tym standardzie! " << endl;
						cout << "Wybierz inna opcje." << endl;
						continue;
					}
					cout << "Liczba dostepnych pokoi 'premium': " << liczba_pokoi[2] << endl << endl;
					nazwa_standard = "premium";
					break;
				default:
					cout << "Podano niepoprawną wartość!" << endl << endl;
					return 0;
			}

			string insert = "INSERT INTO Informacje_hotel (standard_pokoju, cena_doba) VALUES (?, ?);";
    		sqlite3_stmt* stmt2;
			if(sqlite3_prepare_v2(db, insert.c_str(), -1, &stmt2, nullptr) != SQLITE_OK) 
        	{
            	cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
            	return -1;
        	}
        	sqlite3_bind_text(stmt2, 1, nazwa_standard.c_str(), -1, SQLITE_STATIC);
        	sqlite3_bind_int(stmt2, 2, cena_doba);
			if(sqlite3_step(stmt2) != SQLITE_DONE)
        	{
            	cerr << "Błąd przy wykonywaniu zapytania: " << sqlite3_errmsg(db) << endl;
            	sqlite3_finalize(stmt2);
            	return -1;
        	}
        	sqlite3_finalize(stmt2);

			// Transaction/zarzadzanie transakcja wykorzystane w celu unikniecia problemu
			// z usuwaniem ostatniego wiersza tabeli
			sqlite3_exec(db, "BEGIN TRANSACTION;" , nullptr, nullptr, nullptr);
			int id_ostatni_wiersz = sqlite3_last_insert_rowid(db);
			string aktualizacja =  "UPDATE Informacje_hotel SET standard_pokoju = ?, cena_doba = ? WHERE ROWID IN (SELECT ROWID FROM Informacje_hotel ORDER BY ROWID DESC LIMIT 2);"; //WHERE ROWID = " + to_string(id_ostatni_wiersz) + "
    		sqlite3_stmt* stmt_akt;
			if(sqlite3_prepare_v2(db, aktualizacja.c_str(), -1, &stmt_akt, nullptr) != SQLITE_OK) 
        	{
            	cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
				sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
				sqlite3_close(db);
            	return -1;
        	}
			sqlite3_bind_text(stmt_akt, 1, nazwa_standard.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(stmt_akt, 2, cena_doba);
			sqlite3_bind_int(stmt_akt, 3, id_ostatni_wiersz - 1);	
			if(sqlite3_step(stmt_akt) != SQLITE_DONE)
        	{
            	cerr << "Błąd przy wykonywaniu zapytania: " << sqlite3_errmsg(db) << endl;
				sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            	sqlite3_finalize(stmt_akt);
				sqlite3_close(db);
            	return -1;
        	}	
			sqlite3_finalize(stmt_akt);	

			id_ostatni_wiersz = sqlite3_last_insert_rowid(db) + 1;
			string usuwanie =  "DELETE FROM Informacje_hotel WHERE ROWID = (SELECT MAX(ROWID) FROM Informacje_hotel);"; //ORDER BY ROWID DESC LIMIT 1
	    	sqlite3_stmt* stmt_usuwanie;
			if(sqlite3_prepare_v2(db, usuwanie.c_str(), -1, &stmt_usuwanie, nullptr) != SQLITE_OK) 
        	{
            	cerr << "Nie można przygotować zapytania: " << sqlite3_errmsg(db) << endl;
				sqlite3_close(db);
            	return -1;
        	}
			sqlite3_bind_text(stmt_usuwanie, 1, nazwa_standard.c_str(), -1, SQLITE_STATIC);
	        sqlite3_bind_int(stmt_usuwanie, 2, cena_doba);
			if(sqlite3_step(stmt_usuwanie) != SQLITE_DONE)
        	{
            	cerr << "Błąd przy wykonywaniu zapytania: " << sqlite3_errmsg(db) << endl;
            	sqlite3_finalize(stmt_usuwanie);
				sqlite3_close(db);
            	return -1;
        	}	
			sqlite3_finalize(stmt_usuwanie);
			sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
			sqlite3_close(db);
        	return 0;
		}
	}

	// Destruktor, zamkniecie bazy danych
	~Hotel() 
	{ 
        if(db) 
        	sqlite3_close(db);
    }
};

int main()
{	//poprawic estetyke tych komentarzy
	Gosc hotel("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3");
    vector<string> wyniki = hotel.zobacz_oferty_hoteli(); 
    for(const string& wynik : wyniki)
        cout << wynik << endl;
	czekaj(1);


	// Klasa Hotel, Metody: wyswietl_informacje_o_hotelu(), wyswietl_dostepne_pokoje()
	Hotel h("/mnt/c/Users/maksy/OneDrive - Akademia Górniczo-Hutnicza im. Stanisława Staszica w Krakowie/Pulpit/sklonowane/Hotel/data/program_glowna_bd.sqlite3"); 
	h.wyswietl_informacje_o_hotelu(); 
	czekaj(1);
	h.wyswietl_dostepne_pokoje();
	czekaj(1);

	Rezerwacja rez;
	rez.utworz_rezerwacje(); //e.h.  poprawnosc daty ograniczenia etc
	czekaj(1);
	rez.aktualizuj_rezerwacje();
	czekaj(1);
	

	Rachunek rach;
	rach.utworz_rachunek();
	czekaj(1);
	//rach.zaplac(); // nie uwzglednia miesiecy bo 30.01-20.02 to  -10dni
	czekaj(1);
	

	string imie = "nie", nazwisko = "tak", stanowisko = "recepcjonista";
	double id_pracownika;
	Pracownik p(imie, nazwisko, stanowisko, id_pracownika);
	p.aktualizuj_dostepnosc();
	czekaj(1);
	p.zarejstruj_godziny_pracy();
	czekaj(1);

	return 1;
}


//zrobic: czas_pobytu i kwota, czas pracy logika i najlepiej minuty igarnac-->latwije wtedy,
//data logika, przy wpisywaniu do tabel, i wszedzie, zmieniac "."  ma "-"
//jak administrator chce usuwac konta to pewnie niech wywala pracownikow lub poszukac innego (albo gdzies do tych gosci czy rezerwacji dorzucic id gosci i wtedy0 - zrobione