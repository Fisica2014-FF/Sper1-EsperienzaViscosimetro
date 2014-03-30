
#pragma once//L'equivalente delle Include guards

#include <cmath>
#include <vector>
#include <algorithm>
#include <memory>//Shared_ptr
#include <limits>
#include <string>
#include <map>
#include <unordered_map>
//#include <iostream>
#include <fstream>
#include <sstream>
#include "../utils/NomeDiFile.h"


#ifdef _MIO_DEBUG_

#include <iostream>//Per cerr

//Stampa il nome della variabile
#define VNAME(x) #x
#define VDUMP(x) std::clog << #x << " " << x << std::endl

#endif


//Il mio namespace
namespace mions {

//Classi per l'analisi dei dati statistici
namespace dataAnalisi {
using std::vector;
//using std::shared_ptr;

//Aggiunta classe che legge i file in formato fdat
//L'interfaccia è con la notazione "fd[indice]": gli oggetti della classe File_Fdat,
//se l'indice è un numero i, ritornano l'i-esima riga come vettore
//(Naturalmente in questo caso per righe si ignorano commenti e metadati, che iniziano rispettivamente con # e #%).
//Quindi si comportano come matrici (array multidimensionali) indicizzate da numeri, che contengono le colonne di dati.
//Se invece l'indice è una stringa, recupera i metadati associati a quel tag.
template <class T>
class File_Fdat {
	//std::string formato;

	// Leggi una riga alla volta, se è un commento saltala, altrimenti prosegui:
	// 		se è un metadato aggiungilo alla mappa, sennò memorizza la riga di numeri
	/* Esempi di metadati:
	 * #%MATERIALE:2
	 * #%LUNGHEZZA:500
	 *
	 * Inoltre memorizza la matrice dei dati
	 *
	 */

	//Scoprire perchè chiamare il costruttore dall'altro costruttore non funziona

private:
	File_Fdat(std::string nomeFile) {
		using namespace std;
		ifstream file_form;
		string rigamd;

		stringstream sriga;
		stringstream sindice;

		string indice;
		double dvalore;
		char temp;


		clog << nomeFile << endl;
		clog << "E fin qua...\n";
		file_form.open(nomeFile.c_str());
		if (!(file_form.is_open()))
			throw string("[Errore]: file di dati non aperto") + nomeFile;


		while (getline(file_form,rigamd)) {
			//Controlla se i commenti sono tag
			//Leggi i tag (inizia a (0) e prendi (1) carattere)
			if (rigamd.substr(0,1) == "#") {
				//Se una linea inizia con #% è un metadato, della forma #%(string)INDICE:(double)DATO
				if (rigamd.substr(0,2) == "#%") {
					sriga << rigamd; //Scrivi nello stringstream la stringa, per estrarci le varie robe
					sriga >> temp >> temp; //Togli #%

					sriga >> temp;
					while ( temp !=  ':' ) {
						sindice << temp;
						//Prendi un nuovo carattere
						sriga >> temp;
					};
					//I caratteri rimanenti sono il valore
					sriga >> dvalore;
					//metti lo stringstream dentro la stringa
					sindice >> indice;
					MetaDatiGenerici[indice] = dvalore;
				} else
					continue;
			//Altrimenti leggi i numeri
			} else {
				sriga << rigamd; //Scrivi nello stringstream la stringa, per estrarci la riga di dati
				#ifdef _MIO_DEBUG_
				clog << rigamd << "\n";
				#endif


				vector<T> tempvect;
				T tempnum;
				while (sriga)
				{
					sriga >> tempnum;
					// If perchè sennò, arrivato alla fine, cerca di estrarre un numero, fallisce e quindi pusho
					// di nuovo in tempvect l'ultimo numero
					if (sriga)
						tempvect.push_back(tempnum);
				}

				vColDati.push_back(tempvect);
				//r=r+1;
			}
		sriga.clear();
		sindice.clear();
		}
	}
	std::map<std::string, double> MetaDatiGenerici;
	vector< vector<T> > vColDati;//Matrice dei dati: un vettore di puntatori a dei vettori
public:

	File_Fdat(mions::utils::NomeDiFile nomeFile) : File_Fdat(nomeFile.pathTotale) { };

	//Se l'indice è un numero n, ritorna l'n-esima riga della matrice di numeri
	inline std::vector<T> operator[](int indice) {
		//At lancia una out_of_range_exception se si mette un indice troppo grande o troppo piccolo
		return vColDati.at(indice);
	}

	//Se l'indice è una stringa, ritorna il metadato associato nella map
	inline double operator[](std::string stringaindice){


		auto iterat = MetaDatiGenerici.find(stringaindice);
		if (iterat != MetaDatiGenerici.end())
			return iterat->second;
		else
			throw std::string("[Errore]: usato indice non esistente: ") + stringaindice;
	}

};

///////////////////////
//					 //
//	  VERSIONE 1.3	 //
//					 //
///////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Forward declarations
 */
template <typename> class VarStat;//Forward declaration da usare nella funzione operator<<

template <typename T> const VarStat<T> operator*(const double& , const VarStat<T> );
template <typename T> const VarStat<T> operator*(const VarStat<T> , const double& );
template <typename T> const VarStat<T> potcombo(const VarStat<T>& , const double , const VarStat<T>& , const double );

///////////////////////////////////////////////////////////////////////////////////////////////////
//Versione
template <typename U>
std::ostream& operator <<(std::ostream& os, const VarStat<U>& rhs) {
	using namespace std;

	//Eclipse dà problemi con endl, modifichiamolo temporaneamente
	#define endl "\n"
	os << "Numero dati:                       " << rhs.getNumeroDatiEffettivo() << endl;
	os << "Media:                             " << rhs.getMedia() << endl;
	//cout << "Mediana:                           " << rhs.getMediana() << endl;
	os << "Varianza del campione:             " << rhs.getVarianzaCampione() << endl;
	os << "Deviazione standard campione:      " << rhs.getDeviazioneStandardCamp() << endl;
	os << "Varianza della popolazione:        " << rhs.getVarianzaPopolazione() << endl;
	os << "Deviazione standard popolazione:   " << rhs.getDeviazioneStandardPop() << endl;
	os << "Errore della media:                " << rhs.getErroreMedia() << endl;
	os << "Massimo:                           " << rhs.getMax() << endl;
	os << "Minimo:                            " << rhs.getMin() << endl;
	#undef endl

	return os;

}

//Potenza
//template <typename U>
//const VarStat<U> potcombo(const VarStat<U>& base1, const double& esponente1, const VarStat<U>& base2, const double& esponente2){
//		//Obiettivo: dare gli stessi risultati come se avessi moltiplicato i dati di due insiemi (i dati due a due, non gli insiemi) ma senza un ordine definito tra i due
//		//iNumero_dati = iNumero_dati; Non sto unendo gli insiemi di dati, ma moltiplicando i singoli elementi fra loro
//			//Salvo la media
//		VarStat<U> result;
//		result.dMedia = base1.getMedia() * base2.getMedia();//MOltiplica le medie delle due variabili
//		result.dVarianzaCampione = base1.getMedia()*base1.getMedia()*(.getVarianzaCampione() / (base1.getMedia()*base1.getMedia()) + getVarianzaCampione() / (rhs.getMedia() * rhs.getMedia()) );//Propagazione dell'errore
//		dVarianzaPopolazione = getMedia()*getMedia()*(rhs.getVarianzaPopolazione() / (tMedia*tMedia) + getVarianzaPopolazione() / (rhs.getMedia() * rhs.getMedia()) );
//
//		//La nuova varianza permette di calcolare direttamente la nuova std
//		dDeviazioneStandardCamp = sqrt(getVarianzaCampione());
//		dDeviazioneStandardPop = sqrt(getVarianzaPopolazione());
//
//		// Il massimo della somma è la somma dei due massimi
//		// Worst-case max? Non ben definito, ma se ho due set di dati, il massimo (tra tutte le possibilità) è la somma dei due massimi precedenti
//		// TODO: Casi non maggiori di zero
//		dMax = (getMedia() > 0) ? (getMax() * rhs.getMax()) : ( INFINITY );
//
//		//Idem per il minimo, il minimo "minore" è la somma dei minimi
//		dMin = (getMedia() > 0) ? (getMin() * rhs.getMin()) : ( -INFINITY );
//
//		dErroreMedia = abs(getMedia())*sqrt(pow(rhs.getErroreMedia() / rhs.getMedia(),2) + pow(getErroreMedia() / tMedia,2) );//Propagato come una StDev normale
//		return *this;	//Idiozia ma dicono che serva
//};

//Moltiplicazione a destra per uno scalare
template <typename U>
inline const VarStat<U> operator*(const VarStat<U> lhs, const double& rhs) {
	VarStat<U> result = lhs; // Copia il primo oggettp
	result *= rhs;            // Aggiungici dentro l'altro
	return result;              // Ritorna il risultato
}

//Moltiplicazione a sinistra per uno scalare
template <typename U>
inline const VarStat<U> operator*(const double& lhs, const VarStat<U> rhs) {
	return (rhs*lhs);
}






///////////////////////////////////////////////////////////////////////////////////////////////////
// Classe per l'analisi di UNA variabile statistica offline, completammente riscritta.
template <class T>
class VarStat {
private:

	double dMedia = -INFINITY;
	double dDeviazioneStandardPopo = -INFINITY;
	double dErroreMedia = -INFINITY;
	int iNumero_dati = 0;
	vector<T> dati;
	vector<T> errori;

public:
	//vector<T> vectDati;
	//Funzioni overloaded
	friend std::ostream& operator<<<T>(std::ostream& , const VarStat<T>& );
	friend const VarStat<T> operator*<T>(const VarStat<T> , const double& );
	friend const VarStat<T> operator*<T>(const double& , const VarStat<T> );
	//TODO potcombo
	//friend const VarStat<T> potcombo(const VarStat<T>& , const double , const VarStat<T>& , const double );

	//
	VarStat(T valore) {
		iNumero_dati = 1;
		dMedia = (double)valore;
		dDeviazioneStandardPopo = 0;
		dErroreMedia = 0;
	}

	//
	VarStat(T valore, double DevStdPop, int numDati = 100000) {
		iNumero_dati = numDati;
		dMedia = (double)valore;
		dDeviazioneStandardPopo = DevStdPop;
		dErroreMedia = 0;
	}

	//TODO: Pensarci su
//	VarStat(mions::utils::NomeDiFile nomeDiFile) {
//		if (nomeDiFile.estensione != "fdat") {
//			throw "[Errore]: Usato un file non fdat, l'estensione non è corretta";
//		}
//		VarStat(dati, true);
//	}


	//Costruttore
	VarStat(const vector<T>& aDati, bool eliminaTreSigma = true) {
		using std::vector;
		using std::abs;
		//dati = {1,2,3};//La classe ha una copia del vector! Non dei dati! Copiare un vector non è troppo impegnativo. O no? NOOO!!!
		//int numDatiIniziale = aDati.size();
		//Salva i dati
		dati = aDati;
		//vector<int> ListaDatifuori3Sigma; //0.003 = 100% - 99.7% = percentuale atttesa di fuori sigma, più spazio a caso

		T dVarianzaCampione; 	//Sample variance
		T dVarianzaPopolazione; //Population variance
		T dDeviazioneStandardCamp;

		//Se il vettore è vuoto la random variable è 0 +- 0 Buona idea?
		if (dati.size() == 0) {

			#ifdef _MIO_DEBUG_
			std::clog << "Vettore vuoto, metto la variabile a zero+-zero";
			#endif

			iNumero_dati = 0;
			dMedia = 0;
			dDeviazioneStandardPopo = 0;

			dErroreMedia = 0;
			return;
		}

		//Calcola la media standard (non pesata cioè)
		dMedia=(double)dati[0];
		for(int i=0; i < dati.size(); i++){
			//Media
			dMedia=(i*dMedia+(double)dati[i])/(i+1);
		}

		//Calcola la deviazione del campione
		dVarianzaCampione=pow(((double)dati[0]-dMedia),2);
		for(int i=0; i < dati.size(); i++){
			//Varianza
			dVarianzaCampione=(i*dVarianzaCampione+pow(((double)dati[i]-dMedia),2)) /
					(i+1);
		}


		//se sigma2c=S/N e sigma2p=S/(N-1), allora, sostituendo S e risolvendo, sigma2p=sigma2c*N/(N-1)
		dVarianzaPopolazione = dVarianzaCampione*double(dati.size())/(double(dati.size())-1);

		dDeviazioneStandardPopo = sqrt(dVarianzaPopolazione);
		dDeviazioneStandardCamp = sqrt(dVarianzaCampione);
		iNumero_dati = dati.size();
		////////////////////////////////////////////////////////////////////////////////////////////////////
		//Se eliminaTreSigma è true, rifai i conti togliendo i dati inaccettabili
		unsigned long numCancellazioni = 0;
		if (eliminaTreSigma){
			std::clog << "Elimino i dati oltre 3 sigma...\n" ;
			for (typename vector<T>::const_iterator pDato = dati.begin();
					pDato != dati.end();)
			{
				if (abs(dMedia - *pDato ) >= 3*dDeviazioneStandardPopo) {
					/* Cancelliamo dal Vector i dati inaccettabili. Operazione costosa perchè i dati successivi vengono traslati
					 * indietro, ma è meglio un Vector di una LinkedList perchè i dati possono essere messi nella cache e occuma meno memoria.
					 * erase richiede un iterator, quindi siamo "costretti" a usarlo
					 */
					std::cout << "Eliminato dato: " << *pDato << "\n";

					//Erase restituisce l'iteratore dell'elemento successivo a quelli appena cancellati
					pDato = dati.erase(pDato);

					numCancellazioni = numCancellazioni + 1;
				} else {
					//Passiamo all'elemento successivo
					pDato++;
				}
			}
			std::clog << "Cancellati " << numCancellazioni << " dati\n\n";
		}//EndIf
		////////////////////////////////////////////////////////////////////////////////////////////////////

		if ( numCancellazioni > 0) {
			//Rifacciamo i conti
			dMedia=(double)dati[0];
			for(int i=0; i < dati.size(); i++) {
					dMedia=(i*dMedia+(double)dati[i])/(i+1);

			}

			dVarianzaCampione=pow(((double)dati[0]-dMedia),2);
			for(int i=0; i < dati.size(); i++) {
				//Varianza
				dVarianzaCampione=(i*dVarianzaCampione+pow(((double)dati[i]-dMedia),2)) /
									(i+1);
			}
			dDeviazioneStandardCamp = sqrt(dVarianzaCampione);

			//se sigma2c=S/N e sigma2p=S/(N-1), allora, sostituendo S e risolvendo, sigma2p=sigma2c*N/(N-1)
			dVarianzaPopolazione = dVarianzaCampione*double(dati.size())/(double(dati.size())-1);

		}//EndIf del ricalcolo
		////////////////////////////////////////////////////////////////////////////////////////////////////

		//Deviazione standard popolazione
		dDeviazioneStandardPopo=sqrt(dVarianzaPopolazione);
		iNumero_dati = dati.size();
		dErroreMedia = dDeviazioneStandardPopo / sqrt(iNumero_dati);

	}//Fine costruttore

	//Distruttore
	virtual ~VarStat() = default;//Virtual perchè devono ereditare da questa. Lecito il default? Bè compila

	//Getters
	inline double getMedia() const {return dMedia;};
	//Scarto Quadratico Medio (N)
	inline double getDeviazioneStandardPop() const {return dDeviazioneStandardPopo;};
	//Errore Quadrato Medio N-1
	//double getMediana() ordina i dati come side effect
	//Tolta
	// Errore della media
	inline double getErroreMedia() const {return dErroreMedia;}
	long getNumeroDatiEffettivo() const {return iNumero_dati;}
	//Range della variabile

	//Operatori
	//Somma una variabile statistica a un'altra e memorizzala nella prima. Vedi commento su -=, sotto
	inline VarStat<T>& operator+=(const VarStat<T>& rhs) {
		return *this;
	}

	//Sottrai una variabile statistica a un'altra e memorizzala nella prima. v1 -= v2 è come v.operator-=(v2), quindi le funzioni get, etc qui dentro si riferiscono a v1!!! E rhs.get... a v2

	inline VarStat<T>& operator-=(const VarStat<T>& rhs) {
		return *this;
	}

	//Operatori
	//moltiplica una variabile statistica a un'altra e memorizzala nella prima. Vedi commento su -=, sotto
	inline VarStat<T>& operator*=(const VarStat<T>& rhs) {
		return *this;
	}

	inline VarStat<T>& operator/=(const VarStat<T>& rhs) {
		return *this;
	}





	//Moltiplicazione per scalare, compound assignment. v *= d è come v.operator*=(d), quindi le funzioni get, etc qui dentro si riferiscono a v
	inline VarStat<T>& operator*=(const double& rhs) {
		return *this;
	}

	//Somma di due VarStat
	//Trucchetto per riutilizzare il lavoro svolto con +=
	const VarStat<T> operator+(const VarStat<T> &other) const {
		VarStat<T> result = *this; // Copia il primo oggettp
		result += other;            // Aggiungici dentro l'altro
		return result;              // Ritorna il risultato
	}

	//Sottrazione di due VarStat
	//Trucchetto per riutilizzare il lavoro svolto con -=
	const VarStat<T> operator-(const VarStat<T>& other) const {
		VarStat<T> result = *this; // Copia il primo oggettp
		result -= other;            // Aggiungici dentro l'altro
		return result;              // Ritorna il risultato
	}

	const VarStat<T> operator*(const VarStat<T>& other) const {
		VarStat<T> result = *this; // Copia il primo oggettp
		result *= other;            // Aggiungici dentro l'altro
		return result;              // Ritorna il risultato
	}

	const VarStat<T> operator/(const VarStat<T>& other) const {
		VarStat<T> result = *this; // Copia il primo oggettp
		result /= other;            // Aggiungici dentro l'altro
		return result;              // Ritorna il risultato
	}


	//Moltiplicazione per uno scalare
	//Trucchetto per riutilizzare il lavoro svolto con *= double,
//	const VarStat<T> operator*(const double rhs) const {
//		VarStat<T> result = *this; // Copia il primo oggettp
//		result *= rhs;            // Aggiungici dentro l'altro
//		return result;              // Ritorna il risultato
//	}

	//double dModa=0;

};


}//Fine DataAnalisi

}//Fine del mio namespace
