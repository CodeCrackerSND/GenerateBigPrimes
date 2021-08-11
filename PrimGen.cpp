//***************************************************************************************
// PrimGen.cpp : g�n�re des nombres premiers.
//
// la probabilit� pour qu'un nombre de N bits soit premier est de 1/N*ln 2.
// ex : N = 32  : 1/22.
//      N = 64  : 1/44.
//      N = 128 : 1/88.
//      N = 512 : 1/354.
//***************************************************************************************
#include "StdAfx.h"
#include "FirstPrimes.h"
#include "BigInt.h"
#include <vector>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <math.h>

// taille du g�n�rateur al�atoire (en bits) si possible multiple de 32
#define RANDOM_SIZE		1024

// nombre d'it�rations pour les testes probabilistes
#define	NB_ITER			3


// d�claration des fonctions utilis�es
BigInt	pgcd		(BigInt a, BigInt b);
BigInt	expMod		(BigInt a, BigInt b, BigInt n);
BigInt	expMod2		(BigInt a, BigInt b, BigInt n);
int		jacobien	(BigInt a, BigInt b);
bool	isPrim		(BigInt p);
BigInt	euclide		(BigInt a, BigInt b);
void	calcKey		(BigInt p, BigInt q);
bool	miller		(BigInt n);

//***************************************************************************************
// main : g�n�re des nombres premiers et calcule les param�tres des cl�s de l'algo RSA.
//		  - le prog affiche les nombres test�s, s'il est premier ou non ainsi que le temps
//			mis pour le d�terminer avec les 2 tests probabilistes.
//		  - le prog s'arr�te d�s qu'un nombre premier est trouv�.
//		  - apr�s en avoir trouv� 2, il affiche les param�tres des cl�s RSA.
//		  A chaque pause, appuyer sur une touche pour continuer, s pour avoir les statistiques
//		  et a pour quitter le prog.
//***************************************************************************************
int main(int argc, char* argv[])
{
	int nbPrim = 0;
	int nbTot = 0;
	BigInt::InitRandom();
	while(1)
	{
		bool foundP = false;
		bool foundQ = false;
		BigInt p;
		BigInt q;
		while(!foundP || !foundQ)
		{
			BigInt n = 0;
			while(n.IsEven())
			{
				n = BigInt::Random(RANDOM_SIZE);
				nbTot++;
			}
	
			DWORD deb = GetTickCount();
			bool rep = isPrim(n);
			DWORD fin = GetTickCount();

			DWORD deb2 = GetTickCount();
			bool rep2 = miller(n);
			DWORD fin2 = GetTickCount();

			if(rep)
				nbPrim++;

			printf("%s %s %s %d %d\r\n", n.GetStrValue(), rep ? "true" : "false",
					rep2 ? "true" : "false", fin - deb, fin2 - deb2);

			char c = 0;

			if(rep != rep2)
				c = getch();

			if(rep && !foundP)
			{
				p = n;
				foundP = true;
				c = getch();
			}
			else if(rep && foundP && !foundQ)
			{
				q = n;
				foundQ = true;
				c = getch();
			}


			if(foundP && foundQ)
			{
				calcKey(p, q);
				foundP = false;
				foundQ = false;
				c = getch();
			}

//			c = getch();
			if(c == 'a')
				return 0;
			if(c == 's')
			{
				BigInt::PrintStat();
				printf("NbTot = %d NbPrim = %d taux = %lf\r\n", nbTot, nbPrim,
					100 * ((double) nbPrim) / nbTot);
				if(getch() == 'a')
					return 0;
			}
		}
	}
}



//***************************************************************************************
// isPrim : d�termine de fa�on probabiliste si un nombre est premier.
// entr�e : p : le ,ombre � tester.
// retour : vrai si le nombre est premier, faux sinon.
//---------------------------------------------------------------------------------------
// Remarque : A la fin si le nombre est dit premier, il l'est avec une probabilit� de
//				(1 - 1 / 2^NB_ITER). Exemple avec 10 iterations : 99.90% de chance.
//***************************************************************************************
bool isPrim(BigInt p)
{

	int BitsSize = p.GetSize();
	int min = 0;
	if (BitsSize<PRIME_TABLE_SIZE)
	min = BitsSize;
	else
	min = PRIME_TABLE_SIZE;

	if (p>PRIMES[min-1])
	{
	for (int j=0;j<min;j++)  // check if divisible by small prime
	{
	if ((p % PRIMES[j])==0)
	return false;
	}

	}
	else
	{
	for (int j=0;j<min;j++)
	{
	if (p == PRIMES[j])
	return true;  // p is prime
	}

	}

	// on effectue NB_ITER fois la boucle. 
	BigInt pm1 = p - 1;

	for(int i = 0; i < NB_ITER; i++)
	{
		// recherche d'un nombre a<=p
		BigInt a = BigInt::Random(RANDOM_SIZE);
		while(a >= p)
			a = BigInt::Random(RANDOM_SIZE);

		// si le pgcs de a et p est diff�rent de 1, p n'est pas premier
		if(pgcd(a, p) != 1)
			return false;

		// on v�rifie si les 2 expressions suivantes sont �gales
		BigInt val2 = expMod(a, pm1 >> 1, p);
		if(val2 != 1 && val2 != pm1)
			return false;

		int val1 = jacobien(a, p);
		if(val1 == 1 && val2 != 1)
			return false;
		if(val1 == -1 && val2 != pm1)
			return false;
	}

	// le nombre est consid�r� comme premier
	return true;

}


//***************************************************************************************
// pgcd : d�termine quel est le plus grand diviseur commun entre 2 nombres. Si les 2
//		  nombres sont premiers entre eux, le pgcd vaut 1.
// entr�e : a, b : les 2 nombres � tester.
// retour : le pgcd des 2 nombres.
//***************************************************************************************
BigInt pgcd(BigInt a, BigInt b)
{
	BigInt q = 0;
	while(b != 0)
	{
		q = a;
		a = b;
		b = q.DivEuclide(b);
	}
	return a;
}


//***************************************************************************************
// expMod : r�alise une exponentiation modulaire : a^b modulo n.
//***************************************************************************************
BigInt expMod(BigInt a, BigInt b, BigInt n)
{
	if ((a%n)==0) return 0; // In case a is divisible by n
	BigInt m = a;
	BigInt e = b;
	BigInt x = 1;

	while(e != 0)
	{
		while(e.IsEven())
		{
			e >>= 1;
			m = (m * m) % n;
		}
		e--;
		x = (x * m) % n;
	}
	return x;
}

//***************************************************************************************
// jacobien : calcule le jacobien de a et b.
//***************************************************************************************
int jacobien(BigInt a, BigInt b)
{
	if(a == 0)
		return 0;

	int J = 1;
	while(a != 1)
	{
		if(a.IsEven())
		{
			if( ((b * b - 1) >> 3).IsOdd() )
				J *= -1;

			a >>= 1;
		}
		else
		{
			if( ((a - 1) * (b - 1) >> 2).IsOdd() )
				J *= -1;

			BigInt temp = b % a;
			b = a;
			a = temp;
		}
	}

	return J;
}


//***************************************************************************************
// miller : test de primalit� probabiliste de Miller.
// entr�e : p : le ,ombre � tester.
// retour : vrai si le nombre est premier, faux sinon.
//---------------------------------------------------------------------------------------
// Remarque : A la fin si le nombre est dit premier, il l'est avec une probabilit� de
//				(1 - 1 / 4^NB_ITER). Exemple avec 10 iterations : 99.9999% de chance.
//***************************************************************************************
bool miller(BigInt n)
{
	BigInt n1 = n - 1;
	BigInt m = n1;
	int k = 1;
	while(n1 % (1 << (k + 2)) == 0)
	{
		k += 2;
		m >>= 2;
	}

	for(int i = 0; i < NB_ITER; i++)
	{
		BigInt a = BigInt::Random(RANDOM_SIZE);
		while(a >= n)
			a = BigInt::Random(RANDOM_SIZE);


		BigInt b = expMod(a, m, n);

		if(b % n == 1)
			continue;

		for(int j = 0; j < k; j++)
		{
			if(b % n == n1)
				break;
			else
				b *= b;
		}

		if(j >= k)
			return false;
	}

	return true;
}


//***************************************************************************************
// euclide : r�sout l'�quation ax = 1 modulo b.
//---------------------------------------------------------------------------------------
// Remarque : une v�rification doit �tre faite en sortie car il y a un probl�me pour
//			  certaines valeurs.
//***************************************************************************************
BigInt euclide(BigInt a, BigInt b)
{

	BigInt s = 0;
	BigInt r = 1;
	BigInt q = 0;
	BigInt temp = 0;
	BigInt b_sauv = b;
	BigInt u = 1;

	while(r != 0)
	{
		q = a / b;
		r = a % b;

		temp = s;
		s = u - q * s;
		u = temp;
		
		a = b;
		b = r;
	}
	
	BigInt inv = u;
	if(u < 0)
	{
		inv *= -1;
		inv = b_sauv - (inv % b_sauv);
	}
	else
		inv = u % b_sauv;
	
	return inv;
}


//***************************************************************************************
// calcKey : d�termine les param�tres cl� publique et priv�s pour une utilisation de
//			 l'algorithme RSA.
// entr�e : p, q : 2 nombres premiers.
//---------------------------------------------------------------------------------------
// Remarque : la fonction euclide ayant un petit probl�me, une v�rification est faite en
//			  sortie de cette fonction et d'autres cl�s sont g�n�r�es s'il le faut.
//***************************************************************************************
void calcKey(BigInt p, BigInt q)
{ 
	BigInt n = p * q;
	BigInt f = (p - 1) * (q - 1);

	while(1)
	{
		printf("n = %s\r\n", n.GetStrValue());
		printf("f = %s\r\n", f.GetStrValue());

		BigInt e = BigInt::Random(2 * RANDOM_SIZE);
		while(pgcd(e, f) != 1 && pgcd(e, n) != BigInt(1))
			e = BigInt::Random(2 * RANDOM_SIZE);
		printf("e = %s\r\n", e.GetStrValue());

		BigInt d = euclide(e, f);
		printf("d = %s\r\n", d.GetStrValue());

		// v�rification du calcul (n�cessaire dans certains cas)
		BigInt verif = (d * e) % f;
		if(verif != 1)
		{
			printf("erreur\r\n");
			getch();
		}
		else
		{
			printf("OK\r\n");
			break;
		}
	}
}