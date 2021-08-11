//***************************************************************************************
// BigInt.h :
//				Classe BigInt : gestion de nombres entiers cod�s sur un nombre �lev� de
//								bits.
//---------------------------------------------------------------------------------------
// - Les nombres utilis�s sont sign�s (valeur absolue + bit de signe et non pas en
//	 utilisant le compl�ment � 2).
// - le codage utilise plusieurs entiers non sign�s de 32 bits pour la sauvegarde des
//	 donn�es.
// - le nombre de bits de codage �volue dynamiquement en fonction de la valeur et des 
//	 op�rations math�matiques faites dessus.
//***************************************************************************************

#ifndef AFX_BIGINT_H_INCLUDED_
#define AFX_BIGINT_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// entier non sign� de 32 bits
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
// Membres priv�s.
//=======================================================================================
private :

	// tableau des donn�es , nombre de donn�es, signe
	int		m_NbData;
	int		m_Sign;

	// g�n�rateur al�atoire
	static DWORD	m_dwRand;

	// copie, optimisation
	void Copy(BigInt& val);
	void Optimize();

	// affecte, r�cup�re la taille en bits du nombre
	void SetSize(int nbBits);

	// op�rations arithm�tiques internes
	void Add(BigInt& val, bool bCheckSign = true);
	void Sub(BigInt& val, bool bCheckSign = true);
	void Mul(BigInt& val, bool bCheckSign = true);
	BigInt Div(BigInt& val, bool bCheckSign = true);
	DWORD SimpleDiv(BigInt& num, BigInt& val, BigInt& reste);

	// comparaison et d�calage de bits
	int	 Compare(BigInt& val, bool bCheckSign = true);
	void ShiftBits(int dep);

	// op�rations sur les bits du nombre
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

	// fonctions du g�n�rateur al�atoire
	static BigInt BigIntFromDecStr(char* str);
	static BigInt Random(int nbBits);
	static void InitRandom();

	// exponentielle modulaire
	static BigInt ExpMod(BigInt& a, BigInt& b, BigInt& n);

	// afficha les statistiques des fonctions 
	static void PrintStat();

	// r�cup�re une repr�sentation affichable du nombre
	char* GetStrValue();

	std::string GetDecStrValue();

	// r�cup�re le nombre de bits, de DWORD
	inline int GetNbBits	()	{return GetSize();}
	inline int GetNbDWORD	()	{return m_NbData;}

	// affecte, r�up�res les donn�es
	void SetData(DWORD tab[], int nb);
	void GetData(DWORD tab[], int nb);

	// indique si le nombre est pair ou impair
	inline bool IsEven()	{return (m_TabData[0] & 0x00000001) == 0;}
	inline bool IsOdd()		{return (m_TabData[0] & 0x00000001) == 1;}

	// affectation en profondeur
	BigInt& operator=(BigInt& val);

	// division compl�te (avec quotient et reste)
	inline BigInt DivEuclide(BigInt& val)	{return Div(val);}

	//-----------------------------------------------------------------------------------
	// op�rateurs arithm�tiques sans modification des op�randes
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
	// op�rateurs arithm�tiques avec modification de l'op�rande source
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
	// op�rateurs d'incr�mentation et d�cr�mentation (postfix�s et pr�fix�s)
	inline BigInt& operator++()	{Add(BigInt(1)); return *this;}
	inline BigInt& operator--()	{Sub(BigInt(1)); return *this;}
	inline BigInt operator++(int val)	{BigInt temp = *this; Add(BigInt(1)); return temp;}
	inline BigInt operator--(int val)	{BigInt temp = *this; Sub(BigInt(1)); return temp;}

	//-----------------------------------------------------------------------------------
	//op�rateurs de comparaison
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
	// d�calages de bits sans modification de l'op�rande source
	inline BigInt operator<<(int dep)	{BigInt temp = *this; temp.ShiftBits(dep); return temp;}
	inline BigInt operator>>(int dep)	{BigInt temp = *this; temp.ShiftBits(-dep); return temp;}

	//-----------------------------------------------------------------------------------
	// d�calages de bits avec modification de l'op�rande source
	inline BigInt& operator<<=(int dep)		{ShiftBits(dep); return *this;}
	inline BigInt& operator>>=(int dep)		{ShiftBits(-dep); return *this;}

	//-----------------------------------------------------------------------------------
	// op�rateurs sur les bits sans modification des op�randes
	inline BigInt operator&(BigInt& val)	{BigInt temp = *this; temp.AndBits(val); return temp;}
	inline BigInt operator|(BigInt& val)	{BigInt temp = *this; temp.OrBits(val); return temp;}
	inline BigInt operator^(BigInt& val)	{BigInt temp = *this; temp.XorBits(val); return temp;}

	inline BigInt operator&(const int val)	{return operator&(BigInt(val));}
	inline BigInt operator|(const int val)	{return operator|(BigInt(val));}
	inline BigInt operator^(const int val)	{return operator^(BigInt(val));}

	//-----------------------------------------------------------------------------------
	// op�rateurs sur les bits avec modification de l'op�rande source
	inline BigInt& operator&=(BigInt& val)	{AndBits(val); return *this;}
	inline BigInt& operator|=(BigInt& val)	{OrBits(val); return *this;}
	inline BigInt& operator^=(BigInt& val)	{XorBits(val); return *this;}

	inline BigInt& operator&=(const int val)	{return operator&=(BigInt(val));}
	inline BigInt& operator|=(const int val)	{return operator|=(BigInt(val));}
	inline BigInt& operator^=(const int val)	{return operator^=(BigInt(val));}
};


#endif	// AFX_BIGINT_H_INCLUDED_
