//***************************************************************************************
// BigInt.h :
//				Classe BigInt : gestion de nombres entiers codés sur un nombre élevé de
//								bits.
//---------------------------------------------------------------------------------------
// - Les nombres utilisés sont signés (valeur absolue + bit de signe et non pas en
//	 utilisant le complément à 2).
// - le codage utilise plusieurs entiers non signés de 32 bits pour la sauvegarde des
//	 données.
// - le nombre de bits de codage évolue dynamiquement en fonction de la valeur et des 
//	 opérations mathématiques faites dessus.
//***************************************************************************************

#ifndef AFX_BIGINT_H_INCLUDED_
#define AFX_BIGINT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// entier non signé de 32 bits
typedef unsigned long DWORD;


#include <vector>
#include <string.h>
#include <sstream>
#include <algorithm>
#include <math.h>

//***************************************************************************************
// Classe BigInt.
//***************************************************************************************
class BigInt  
{

//=======================================================================================
// Membres privés.
//=======================================================================================
private :

	// tableau des données , nombre de données, signe
	int		m_NbData;
	int		m_Sign;

	// générateur aléatoire
	static DWORD	m_dwRand;

	// copie, optimisation
	void Copy(BigInt& val);
	void Optimize();

	// affecte, récupère la taille en bits du nombre
	void SetSize(int nbBits);

	// opérations arithmétiques internes
	void Add(BigInt& val, bool bCheckSign = true);
	void Sub(BigInt& val, bool bCheckSign = true);
	void Mul(BigInt& val, bool bCheckSign = true);
	BigInt Div(BigInt& val, bool bCheckSign = true);
	DWORD SimpleDiv(BigInt& num, BigInt& val, BigInt& reste);

	// comparaison et décalage de bits
	int	 Compare(BigInt& val, bool bCheckSign = true);
	void ShiftBits(int dep);

	// opérations sur les bits du nombre
	void AndBits(BigInt& val);
	void OrBits(BigInt& val);
	void XorBits(BigInt& val);

//=======================================================================================
// Membres publics.
//=======================================================================================
public :

	DWORD*	m_TabData;
	// constructeurs, destructeur
	BigInt(int val = 0);
	BigInt(char* szVal);
	BigInt(BigInt& val);
	~BigInt();

	int	 GetSize();

	// fonctions du générateur aléatoire
	static BigInt BigIntFromDecStr(char* str);
	static BigInt Random(int nbBits);
	static void InitRandom();

	// exponentielle modulaire
	static BigInt ExpMod(BigInt& a, BigInt& b, BigInt& n);

	// afficha les statistiques des fonctions 
	static void PrintStat();

	// récupère une représentation affichable du nombre
	char* GetStrValue();

	std::string GetDecStrValue();

	// récupère le nombre de bits, de DWORD
	inline int GetNbBits	()	{return GetSize();}
	inline int GetNbDWORD	()	{return m_NbData;}

	// affecte, réupères les données
	void SetData(DWORD tab[], int nb);
	void GetData(DWORD tab[], int nb);

	// indique si le nombre est pair ou impair
	inline bool IsEven()	{return (m_TabData[0] & 0x00000001) == 0;}
	inline bool IsOdd()		{return (m_TabData[0] & 0x00000001) == 1;}

	// affectation en profondeur
	BigInt& operator=(BigInt& val);

	// division complète (avec quotient et reste)
	inline BigInt DivEuclide(BigInt& val)	{return Div(val);}

	//-----------------------------------------------------------------------------------
	// opérateurs arithmétiques sans modification des opérandes
	inline BigInt operator+(BigInt& val)	{BigInt temp = *this; temp.Add(val); return temp;}
	inline BigInt operator-(BigInt& val)	{BigInt temp = *this; temp.Sub(val); return temp;}
	inline BigInt operator*(BigInt& val)	{BigInt temp = *this; temp.Mul(val); return temp;}
	inline BigInt operator/(BigInt& val)	{BigInt temp = *this; temp.Div(val); return temp;}
	inline BigInt operator%(BigInt& val)	{BigInt temp = *this; return temp.Div(val);}

	inline BigInt operator+(const int val)	{return operator+(BigInt(val));}
	inline BigInt operator-(const int val)	{return operator-(BigInt(val));}
	inline BigInt operator*(const int val)	{return operator*(BigInt(val));}
	inline BigInt operator/(const int val)	{return operator/(BigInt(val));}
	inline BigInt operator%(const int val)	{return operator%(BigInt(val));}

	//-----------------------------------------------------------------------------------
	// opérateurs arithmétiques avec modification de l'opérande source
	inline BigInt& operator+=(BigInt& val)	{Add(val); return *this;}
	inline BigInt& operator-=(BigInt& val)	{Sub(val); return *this;}
	inline BigInt& operator*=(BigInt& val)	{Mul(val); return *this;}
	inline BigInt& operator/=(BigInt& val)	{Div(val); return *this;}
	inline BigInt& operator%=(BigInt& val)	{Copy(Div(val)); return *this;}

	inline BigInt& operator+=(const int val)	{return operator+=(BigInt(val));}
	inline BigInt& operator-=(const int val)	{return operator-=(BigInt(val));}
	inline BigInt& operator*=(const int val)	{return operator*=(BigInt(val));}
	inline BigInt& operator/=(const int val)	{return operator/=(BigInt(val));}
	inline BigInt& operator%=(const int val)	{return operator%=(BigInt(val));}

	//-----------------------------------------------------------------------------------
	// opérateurs d'incrémentation et décrémentation (postfixés et préfixés)
	inline BigInt& operator++()	{Add(BigInt(1)); return *this;}
	inline BigInt& operator--()	{Sub(BigInt(1)); return *this;}
	inline BigInt operator++(int val)	{BigInt temp = *this; Add(BigInt(1)); return temp;}
	inline BigInt operator--(int val)	{BigInt temp = *this; Sub(BigInt(1)); return temp;}

	//-----------------------------------------------------------------------------------
	//opérateurs de comparaison
	inline bool operator==	(BigInt& val)	{return (Compare(val) == 0);}
	inline bool operator!=	(BigInt& val)	{return (Compare(val) != 0);}
	inline bool operator>	(BigInt& val)	{return (Compare(val) > 0);}
	inline bool operator<	(BigInt& val)	{return (Compare(val) < 0);}
	inline bool operator>=	(BigInt& val)	{return (Compare(val) >= 0);}
	inline bool operator<=	(BigInt& val)	{return (Compare(val) <= 0);}

	inline bool operator==	(const int val)	{return (Compare(BigInt(val)) == 0);}
	inline bool operator!=	(const int val)	{return (Compare(BigInt(val)) != 0);}
	inline bool operator>	(const int val)	{return (Compare(BigInt(val)) > 0);}
	inline bool operator<	(const int val)	{return (Compare(BigInt(val)) < 0);}
	inline bool operator>=	(const int val)	{return (Compare(BigInt(val)) >= 0);}
	inline bool operator<=	(const int val)	{return (Compare(BigInt(val)) <= 0);}

	//-----------------------------------------------------------------------------------
	// décalages de bits sans modification de l'opérande source
	inline BigInt operator<<(int dep)	{BigInt temp = *this; temp.ShiftBits(dep); return temp;}
	inline BigInt operator>>(int dep)	{BigInt temp = *this; temp.ShiftBits(-dep); return temp;}

	//-----------------------------------------------------------------------------------
	// décalages de bits avec modification de l'opérande source
	inline BigInt& operator<<=(int dep)		{ShiftBits(dep); return *this;}
	inline BigInt& operator>>=(int dep)		{ShiftBits(-dep); return *this;}

	//-----------------------------------------------------------------------------------
	// opérateurs sur les bits sans modification des opérandes
	inline BigInt operator&(BigInt& val)	{BigInt temp = *this; temp.AndBits(val); return temp;}
	inline BigInt operator|(BigInt& val)	{BigInt temp = *this; temp.OrBits(val); return temp;}
	inline BigInt operator^(BigInt& val)	{BigInt temp = *this; temp.XorBits(val); return temp;}

	inline BigInt operator&(const int val)	{return operator&(BigInt(val));}
	inline BigInt operator|(const int val)	{return operator|(BigInt(val));}
	inline BigInt operator^(const int val)	{return operator^(BigInt(val));}

	//-----------------------------------------------------------------------------------
	// opérateurs sur les bits avec modification de l'opérande source
	inline BigInt& operator&=(BigInt& val)	{AndBits(val); return *this;}
	inline BigInt& operator|=(BigInt& val)	{OrBits(val); return *this;}
	inline BigInt& operator^=(BigInt& val)	{XorBits(val); return *this;}

	inline BigInt& operator&=(const int val)	{return operator&=(BigInt(val));}
	inline BigInt& operator|=(const int val)	{return operator|=(BigInt(val));}
	inline BigInt& operator^=(const int val)	{return operator^=(BigInt(val));}
};


#endif	// AFX_BIGINT_H_INCLUDED_
