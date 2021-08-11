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

#include "StdAfx.h"
#include "BigInt.h"
#include "string.h"

// la définition de cette constante permet d'avoir des statistiques sur les appels aux
// fonctions de base ainsi que leur temps d'exécution
//#define USE_STAT

#ifdef USE_STAT

static DWORD NbAdd		= 0;
static DWORD NbSub		= 0;
static DWORD NbMul		= 0;
static DWORD NbDiv		= 0;
static DWORD NbComp		= 0;
static DWORD NbShift	= 0;
static DWORD NbCopy		= 0;
static DWORD NbOpt		= 0;

static DWORD TimeAdd	= 0;
static DWORD TimeSub	= 0;
static DWORD TimeMul	= 0;
static DWORD TimeDiv	= 0;
static DWORD TimeComp	= 0;
static DWORD TimeShift	= 0;
static DWORD TimeCopy	= 0;
static DWORD TimeOpt	= 0;


#endif	// USE_STAT


//***************************************************************************************
// Membres statics.
//***************************************************************************************
DWORD BigInt::m_dwRand = 0;


//***************************************************************************************
//***************************************************************************************
void BigInt::PrintStat()
{
#ifdef USE_STAT
	printf("Add   : %9d %7d %10.6lf\r\n", NbAdd, TimeAdd, ((double) TimeAdd) / NbAdd);
	printf("Sub   : %9d %7d %10.6lf\r\n", NbSub, TimeSub, ((double) TimeSub) / NbSub);
	printf("Mul   : %9d %7d %10.6lf\r\n", NbMul, TimeMul, ((double) TimeMul) / NbMul);
	printf("Div   : %9d %7d %10.6lf\r\n", NbDiv, TimeDiv, ((double) TimeDiv) / NbDiv);
	printf("Comp  : %9d %7d %10.6lf\r\n", NbComp, TimeComp, ((double) TimeComp) / NbComp);
	printf("Shift : %9d %7d %10.6lf\r\n", NbShift, TimeShift, ((double) TimeShift) / NbShift);
	printf("Copy  : %9d %7d %10.6lf\r\n", NbCopy, TimeCopy, ((double) TimeCopy) / NbCopy);
	printf("Opt   : %9d %7d %10.6lf\r\n", NbOpt, TimeOpt, ((double) TimeOpt) / NbOpt);
#endif	// USE_STAT
}


//***************************************************************************************
// Constructeur : initialise le nombre à un entier signé de 32 bits.
// entrée : val : valeur initiale à affecter (0 par défaut).
//***************************************************************************************
BigInt::BigInt(int val)
{
	// init des paramètres, création du tableau et affectattion de la valeur
	m_TabData = NULL;
	SetSize(32);
	if(val >= 0)
	{
		m_TabData[0] = val;
		m_Sign = 1;
	}
	else
	{
		m_TabData[0] = -val;
		m_Sign = -1;
	}
}

//***************************************************************************************
// Constructeur : initialise le nombre à partir d'un autre.
// entrée : val : valeur initiale à affecter.
//***************************************************************************************
BigInt::BigInt(BigInt& val)
{
	// on recopie la valeur
	m_TabData = NULL;
	Copy(val);
}


//***************************************************************************************
// Constructeur : initialise le nombre à partir d'une chaîne de caractère.
// entrée : szVal : chaîne de caractères représentant le nombre en hexa. Les caractères
//					reconnus sont : 0-9, a-f, A-F. Les autres caractères sont pris comme
//					étant 0.
//***************************************************************************************
BigInt::BigInt(char* szVal)
{
	// si pas de chaîne fournie, appeler le constructeur de base
	if(szVal == NULL)
	{
		BigInt(0);
		return;
	}

	// taille de la chaîne et nombre de bits (4 par carcatères)
	int length = strlen(szVal);
	m_TabData = NULL;
	SetSize(4 * length);

	// affectation des données (on commence par les bits de poids faible
	for(int i = 0; i < length; i++)
	{
		int numData = i / 8;
		int numBit	= 4 * (i % 8);
		int data = 0;
		char c = szVal[length - 1 - i];
		if(c >= '0' && c <= '9')
			data = c - '0';
		else if(c >= 'a' && c <= 'f')
			data = c - 'a' + 10;
		else if(c >= 'A' && c <= 'F')
			data = c - 'A' + 10;

		m_TabData[numData] |= data << numBit;
	}

	unsigned int n1 = m_TabData[0];
	unsigned int n2 = m_TabData[1];
	// optimisation
	Optimize();
}

// BigInt from decimal string:
BigInt BigInt::BigIntFromDecStr(char* str)
{
	int length = strlen(str);
	BigInt vnum = 0;

	for (int i=0;i<length;i++)
	{
	vnum *= 10;
	vnum += str[i]-'0';	
	}

	return vnum;
		
}

	std::string BigInt::GetDecStrValue()
	{
	BigInt numb = *this;
	//if (IsZero(num))
	//return "0";

	std::string result;
	    do {
        result += (int)(numb.m_TabData[0]%10)+'0';
		numb /= 10;
        //div(num, 10);
    } while(numb.m_NbData>1||numb.m_TabData[0]>0);

    std::reverse(result.begin(), result.end());
    return result;

	}


//***************************************************************************************
// Destructeur.
//***************************************************************************************
BigInt::~BigInt()
{
	// destruction du tableau (il existe obligatoirement)
	delete[] m_TabData;
}


//***************************************************************************************
//***************************************************************************************
void BigInt::SetData(DWORD tab[], int nb)
{
	// affectation de la taille, recopie des données et optimisation
	SetSize(nb * 32);
	memcpy(m_TabData, tab, nb * sizeof(DWORD));
	Optimize();
}


//***************************************************************************************
//***************************************************************************************
void BigInt::GetData(DWORD tab[], int nb)
{
	// vérif de la taille fournie et recopie des données
	if(nb != m_NbData)
		return;
	memcpy(tab, m_TabData, nb * sizeof(DWORD));
}


//***************************************************************************************
//***************************************************************************************
BigInt BigInt::Random(int nbBits)
{

	BigInt val;
	val.SetSize(nbBits);
	for(int i = 0; i < val.m_NbData; i++)
	{
		m_dwRand = m_dwRand * 214013L + 2531011L;
		DWORD dwRand1 = m_dwRand >> 16;
		m_dwRand = m_dwRand * 214013L + 2531011L;
		DWORD dwRand2 = m_dwRand >> 16;
		val.m_TabData[i] = (dwRand1 << 16) | dwRand2;
	}

	// on masque les bits de poids fort inutiles
	int nb = nbBits % 32;
	if(nb != 0)
	{
		val.m_TabData[val.m_NbData - 1] <<= (32 - nb);
		val.m_TabData[val.m_NbData - 1] >>= (32 - nb);
	}

	// optimisation et retour
	val.Optimize();
	return val;
}


//***************************************************************************************
// InitRandom : initialisation du générateur aléatoire.
//***************************************************************************************
void BigInt::InitRandom()
{
	m_dwRand = GetTickCount() ^ GetCurrentProcessId();
}


//***************************************************************************************
// GetStrValue : récupère une représentation en héxa du nombre.
//***************************************************************************************
char* BigInt::GetStrValue()
{
	static char szVal[2048];
	for(int i = m_NbData - 1; i >= 0; i--)
		sprintf(szVal + (m_NbData - i - 1) * 8, "%08X", m_TabData[i]);

	return szVal;
}


//***************************************************************************************
// SetSize : modifie la taille du nombre et l'initialise à 0.
// entrée : la taille du nombre (en bits).
//***************************************************************************************
void BigInt::SetSize(int nbBits)
{
	// destruction ancien tableau (s'il existe)
	if(m_TabData)
		delete[] m_TabData;

	// nombre de données nécessaires
	m_NbData = nbBits / 32;
	if(nbBits % 32 != 0)
		m_NbData++;

	// création tableau et init à 0
	m_TabData = new DWORD[m_NbData];
	memset(m_TabData, 0, m_NbData * sizeof(DWORD));
	m_Sign = 1;
}


//***************************************************************************************
// GetSize : récupère la taille du nombre.
// retour : la taille du nombre (en bits).
//***************************************************************************************
int BigInt::GetSize()
{
	// nombre de données réellement utiles
	int nbData = m_NbData;
	while(nbData > 1 && m_TabData[nbData - 1] == 0)
		nbData--;

	// nombre de bits de poids fort non nul
	int i = 32;
	while(i > 1 && (m_TabData[nbData - 1] << (32 - i)) >> 31 == 0)
		i--;

	// retour
	return 32 * (nbData - 1) + i;
}


//***************************************************************************************
// Copy : recopie la valeur d'un nombre.
// entrée : val : le nombre à copier.
//***************************************************************************************
void BigInt::Copy(BigInt& val)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// affectation du nombre de données
	m_NbData = val.m_NbData;

	// création tableau et recopie des données
	m_TabData = new DWORD[m_NbData];
	memcpy(m_TabData, val.m_TabData, m_NbData * sizeof(DWORD));
	m_Sign = val.m_Sign;

#ifdef USE_STAT
		TimeCopy += GetTickCount() - deb;
		NbCopy++;
#endif	// USE_STAT

}


//***************************************************************************************
// Add : ajoute un nombre.
// entrée : val			: le nombre à ajouter.
//			bCheckSign	: vrai s'il faut prendre en compte les signes, faux sinon.
//---------------------------------------------------------------------------------------
// Remarque : - la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::Add(BigInt& val, bool bCheckSign)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// vérif des signes
	if(bCheckSign)
	{
		if(m_Sign != val.m_Sign)
		{
			// réalisation d'une soustraction non signée
			if(Compare(val, false) >= 0)
			{
				// le signe final est celui de l'appelant
				int sign = m_Sign;
				Sub(val, false);
				m_Sign = sign;
			}
			else
			{
				// le signe final est celui du nombre de droite
				BigInt temp = val;
				temp.Sub(*this, false);
				Copy(temp);
				m_Sign = val.m_Sign;
			}
		}
		else
		{
			// addition non signé, le signe final est celui du nombre appelant
			int sign = m_Sign;
			Add(val, false);
			m_Sign = sign;
		}

		// retour (l'optimisation a déjà été faite)
#ifdef USE_STAT
		TimeAdd += GetTickCount() - deb;
		NbAdd++;
#endif	// USE_STAT
		return;
	}

	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeurs et affectation de la taille du résultat
	BigInt temp = *this;
	SetSize(max(size1, size2) + 1);

	DWORD retenue = 0;
	for(int i = 0; i < m_NbData; i++)
	{
		// premier nombre de la somme partielle
		unsigned __int64 op1 = 0;
		if(i < temp.m_NbData)
			op1 = temp.m_TabData[i];

		// second nombre de la somme partielle
		unsigned __int64 op2 = 0;
		if(i < val.m_NbData)
			op2 = val.m_TabData[i];
		
		// résultat partiel et retenue
		unsigned __int64 result	= op1 + op2 + retenue;
		m_TabData[i] = (DWORD)(result & 0x00000000FFFFFFFF);
		retenue = (DWORD) (result >> 32);
	}

	// optimisation du résultat
	Optimize();

#ifdef USE_STAT
	TimeAdd += GetTickCount() - deb;
	NbAdd++;
#endif	// USE_STAT
}


//***************************************************************************************
// Sub : soustrait un nombre.
// entrée : val : le nombre à soustraire.
//			bCheckSign	: vrai s'il faut prendre en compte les signes, faux sinon.
//---------------------------------------------------------------------------------------
// Remarque : - la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::Sub(BigInt& val, bool bCheckSign)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// vérif des signes
	if(bCheckSign)
	{
		if(m_Sign != val.m_Sign)
		{
			// réalisation d'une addition non signée, le signe final est celui du nombre
			// appelant
			int sign = m_Sign;
			Add(val, false);
			m_Sign = sign;
		}
		else
		{
			// réalisation d'une soustraction non signée (le plus grand - le plus petit)
			if(Compare(val, false) >= 0)
			{
				// le signe final est celui de l'appelant
				int sign = m_Sign;
				Sub(val, false);
				m_Sign = sign;
			}
			else
			{
				// le signe final est l'opposé de celui du nombre de droite
				BigInt temp = val;
				temp.Sub(*this, false);
				Copy(temp);
				m_Sign = -val.m_Sign;
			}
		}

		// retour (l'optimisation a déjà été faite)
#ifdef USE_STAT
		TimeSub += GetTickCount() - deb;
		NbSub++;
#endif	// USE_STAT
		return;
	}

	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeurs et affectation de la taille du résultat
	BigInt temp = *this;
	SetSize(max(size1, size2));

	DWORD retenue = 0;
	for(int i = 0; i < m_NbData; i++)
	{
		// premier nombre de la soustraction partielle
		unsigned __int64 op1 = 0;
		if(i < temp.m_NbData)
			op1 = temp.m_TabData[i];

		// second nombre de la soustraction partielle
		unsigned __int64 op2 = 0;
		if(i < val.m_NbData)
			op2 = val.m_TabData[i];

		if(op1 < op2 + retenue)
			retenue += (1 << 32);
		
		// résultat partiel et retenue
		unsigned __int64 result;
		if(op1 >= op2 + retenue)
		{
			result = op1 - op2 - retenue;
			retenue = 0;
		}
		else
		{
			result = op1 + (1 << 32) - op2 - retenue;
			retenue = 1;
		}

		// affectation
		m_TabData[i] = (DWORD)(result & 0x00000000FFFFFFFF);
	}

	// optimisation du résultat
	Optimize();

#ifdef USE_STAT
	TimeSub += GetTickCount() - deb;
	NbSub++;
#endif	// USE_STAT
}


//***************************************************************************************
// Mul : multiplue par un nombre.
// entrée : val			: le nombre par lequel multiplier.
//			bCheckSign	: vrai s'il faut prendre en compte les signes, faux sinon.
//---------------------------------------------------------------------------------------
// Remarque : la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::Mul(BigInt& val, bool bCheckSign)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeur et affectation de la taille du résultat
	BigInt mul1;
	BigInt mul2;
	if (size1 > size2)
	{
		mul1 = *this;
		mul2 = val;
	}
	else
	{
		mul1 = val;
		mul2 = *this;
	}
	SetSize(size1 + size2);

	// calcul
	for (int i = 0; i < mul2.m_NbData; i++)
	{
		// résultat partiel et retenue
		BigInt partiel;
		partiel.SetSize(size1 + size2);
		DWORD retenue = 0;
		for (int j = 0; j < mul1.m_NbData; j++)
		{
			// calcul
unsigned __int64 res = (unsigned __int64)mul1.m_TabData[j]*(unsigned __int64)mul2.m_TabData[i] + retenue;

			// affectation et nouvelle retenue
			partiel.m_TabData[j] = (DWORD) (res & 0x00000000FFFFFFFF);
			retenue = (DWORD) (res >> 32);
		}

		// ajout retenue
		if (retenue != 0)
			partiel.m_TabData[j] = retenue;

		// décalage et ajout au résultat global
		partiel.ShiftBits(32 * i);
		Add(partiel, false);
	}

	// détermination du signe du résultat (si nécessaire)
	if (bCheckSign)
		m_Sign = mul1.m_Sign * mul2.m_Sign;

	// optimisation du résultat
	Optimize();

#ifdef USE_STAT
	TimeMul += GetTickCount() - deb;
	NbMul++;
#endif	// USE_STAT

}


//***************************************************************************************
//***************************************************************************************
DWORD BigInt::SimpleDiv(BigInt& num, BigInt& val, BigInt& reste)
{
	int comp = num.Compare(val, false);

	if(comp == -1)
	{
		reste.Copy(num);
		return 0;
	}
	if(comp == 0)
	{
		reste.SetSize(32);
		reste.m_TabData[0] = 0;
		return 1;
	}

	unsigned __int64 temp = num.m_TabData[num.m_NbData - 1];
	if(num.m_NbData == val.m_NbData + 1)
		temp = (temp << 32) | num.m_TabData[num.m_NbData - 2];

	// quotient
	BigInt quotient;
	quotient.SetSize(32);
	quotient.m_TabData[0] = (DWORD) (temp / val.m_TabData[val.m_NbData - 1]);

	BigInt mul = val;
	mul.Mul(quotient, false);
	while(mul.Compare(num, false) == 1)
	{
		quotient.m_TabData[0]--;
		mul = val;
		mul.Mul(quotient, false);
	}

	// reste
	reste = num;
	reste.Sub(mul, false);
	return quotient.m_TabData[0];
}


//***************************************************************************************
// Div : effectue une division complète.
// entrée : val			: le nombre par lequel diviser.
//			bCheckSign	: vrai s'il faut prendre en compte les signes, faux sinon.
// retour : le reste de la division.
//---------------------------------------------------------------------------------------
// Remarque : la taille des nombres est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
BigInt BigInt::Div(BigInt& val, bool bCheckSign)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// si le numérateur est inférieur au diviseur on affecte 0 et on retourne le numérateur
	int comp = Compare(val, false);
	if(comp == -1)
	{
		BigInt temp = *this;
		SetSize(32);
		m_TabData[0] = 0;

#ifdef USE_STAT
		TimeDiv += GetTickCount() - deb;
		NbDiv++;
#endif	// USE_STAT

		return temp;
	}

	// si le numérateur est égal au diviseur on affecte 1 et on retourne 0
	if(comp == 0)
	{
		SetSize(32);
		m_TabData[0] = 1;

		// signe du quotient
		if(bCheckSign)
			m_Sign *= val.m_Sign;

#ifdef USE_STAT
		TimeDiv += GetTickCount() - deb;
		NbDiv++;
#endif	// USE_STAT

		return BigInt(0);
	}

	// sauvegarde numérateur, récupération des tailles, init du résultat et du reste
	BigInt temp = *this;
	int sign = m_Sign;
	temp.Optimize();
	int size1 = GetSize();
	int size2 = val.GetSize();
	SetSize(32);
	BigInt reste;

	for(int i = temp.m_NbData - 1; i >= 0; i--)
	{
		reste.ShiftBits(32);
		reste.m_TabData[0] = temp.m_TabData[i];
		if(reste.Compare(val, false) == -1)
			continue;

		DWORD quotient = SimpleDiv(reste, val, reste);
		ShiftBits(32);
		m_TabData[0] = quotient;
	}

	reste.Optimize();
	Optimize();
	
	// signe du résultat
	if(bCheckSign)
	{
		reste.m_Sign = sign;
		m_Sign = sign * val.m_Sign;
	}


	// on retourne le reste
#ifdef USE_STAT
	TimeDiv += GetTickCount() - deb;
	NbDiv++;
#endif	// USE_STAT
	return reste;
}

	
//***************************************************************************************
// Compare : compare avec un nombre.
// entrée : val			: nombre auquel comparer.
//			bCheckSign	: vrai s'il faut prendre en compte les signes, faux sinon.
// retour : -1 si le nombre passé en paramètre est plus grand.
//			0  si le nombre passé en paramètre est égal.
//			+1 le nombre passé en paramètre est plus petit.
//***************************************************************************************
int BigInt::Compare(BigInt& val, bool bCheckSign)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// vérif des signes
	if(bCheckSign)
	{
		if(m_Sign != val.m_Sign)
		{
#ifdef USE_STAT
			TimeComp += GetTickCount() - deb;
			NbComp++;
#endif	// USE_STAT

			return ((m_Sign > val.m_Sign) ? +1 : -1);
		}
	}

	// facteur de sign
	int sign = 1;
	if(bCheckSign)
		sign = m_Sign;

	// on commence par comparer les tailles
	int size1 = GetSize();
	int size2 = val.GetSize();
	if(size1 != size2)
	{
#ifdef USE_STAT
		TimeComp += GetTickCount() - deb;
		NbComp++;
#endif	// USE_STAT

		return ((size1 > size2) ? +sign : -sign);
	}

	// on compare les données à partir du poids fort, et on s'arrête dès qu'elles diffèrent
	for(int i = m_NbData - 1; i >= 0; i--)
	{
		if(m_TabData[i] != val.m_TabData[i])
		{
#ifdef USE_STAT
			TimeComp += GetTickCount() - deb;
			NbComp++;
#endif	// USE_STAT

			return ((m_TabData[i] > val.m_TabData[i]) ? +sign : -sign);
		}
	}

	// les 2 nombres sont égaux
#ifdef USE_STAT
	TimeComp += GetTickCount() - deb;
	NbComp++;
#endif	// USE_STAT
	return 0;
}


//***************************************************************************************
// Optimize : optimise le nombre de bits de codage en supprimant les bits de poids fort
//			  qui sont à 0.
//***************************************************************************************
void BigInt::Optimize()
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// nombre de données réellement utiles
	int nbNewData = m_NbData;
	while(nbNewData > 1 && m_TabData[nbNewData - 1] == 0)
		nbNewData--;

	// si toutes les données sont utiles
	if(nbNewData == m_NbData)
	{
#ifdef USE_STAT
		TimeOpt += GetTickCount() - deb;
		NbOpt++;
#endif	// USE_STAT

		return;
	}

	// ne récupérer que les données utiles
	m_NbData = nbNewData;
	DWORD* tabNewData = new DWORD[m_NbData];
	for(int i = 0; i < m_NbData; i++)
		tabNewData[i] = m_TabData[i];

	if(m_NbData == 0 && m_TabData[0] == 0)
		m_Sign = 1;

	// détruire l'ancien tableau et affecter le nouveau
	delete[] m_TabData;
	m_TabData = tabNewData;

#ifdef USE_STAT
		TimeOpt += GetTickCount() - deb;
		NbOpt++;
#endif	// USE_STAT

}


//***************************************************************************************
// ShiftBits : effectue un déplacement des bits.
// entrée : dep : le déplacement à effectuer :
//				  - >0 pour un déplacement à gauche.
//				  - <0 pour un déplacement à droite.
//---------------------------------------------------------------------------------------
// Remarque : la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::ShiftBits(int dep)
{
#ifdef USE_STAT
	DWORD deb = GetTickCount();
#endif	// USE_STAT

	// déplacement positif (décalage à gauche)
	if(dep > 0)
	{
		// sauvegarder le nombre et modifier la taille du tableau des données
		BigInt temp = *this;
		temp.Optimize();
		SetSize(temp.GetSize() + dep);

		// nombre de données entière à décaler (on le fait même si nb est nul pour recopier
		// le tableau de données
		int nb = dep / 32;
		for(int i = temp.m_NbData - 1; i >= 0; i--)
			m_TabData[i + nb] = temp.m_TabData[i];

		// décalages des derniers bits (on utilise des entiers de 64 bits pour ne perdre
		// aucune données, sauf pour le poids fort)
		nb = dep % 32;
		if(nb != 0)
		{
			m_TabData[m_NbData - 1] <<= nb;
			for(int i = m_NbData - 2; i >= 0; i--)
			{
				unsigned __int64 val = m_TabData[i];
				val <<= nb;
				m_TabData[i + 1] |= ((val & 0xFFFFFFFF00000000) >> 32);
				m_TabData[i] <<= nb;
			}
		}

	}

	// déplacement négatif (décalage à droite)
	if(dep < 0)
	{
		dep = -dep;
		// nombre de données entière à décaler
		int nb = dep / 32;
		if(nb != 0)
		{
			for(int i = 0; i < m_NbData; i++)
			{
				if(i < m_NbData - nb)
					m_TabData[i] = m_TabData[i + nb];
				else
					m_TabData[i] = 0;
			}
		}

		// décalages des derniers bits (on utilise des entiers de 64 bits pour ne perdre
		// aucune données, sauf pour le poids fort)
		nb = dep % 32;
		if(nb != 0)
		{
			m_TabData[0] >>= nb;
			for(int i = 1; i < m_NbData; i++)
			{
				unsigned __int64 val = m_TabData[i];
				val = (val << 32) >> nb;
				m_TabData[i - 1] |= (val & 0x00000000FFFFFFFF);
				m_TabData[i] >>= nb;
			}
		}
	}

	// optimisation de la taille des nombres
	if(dep != 0)
		Optimize();

#ifdef USE_STAT
	TimeShift += GetTickCount() - deb;
	NbShift++;
#endif	// USE_STAT
}


//***************************************************************************************
// AndBits : effectue un ET des bits.
// entrée : val : nombre avec lequel réaliser l'opération
//---------------------------------------------------------------------------------------
// Remarque : la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::AndBits(BigInt& val)
{
	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeurs et modif de la taille
	if(size1 < size2)
	{
		BigInt temp = *this;
		SetSize(size2);
		memcpy(m_TabData, temp.m_TabData, temp.m_NbData * sizeof(DWORD));
	}

	// on effectue le AND bit à bit
	for(int i = 0; i < m_NbData; i++)
	{
		if(i < val.m_NbData)
			m_TabData[i] &= val.m_TabData[i];
		else
			m_TabData[i] = 0;
	}

	// optimisation du résultat
	Optimize();
}


//***************************************************************************************
// OrBits : effectue un OU des bits.
// entrée : val : nombre avec lequel réaliser l'opération
//---------------------------------------------------------------------------------------
// Remarque : la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::OrBits(BigInt& val)
{
	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeurs et modif de la taille
	if(size1 < size2)
	{
		BigInt temp = *this;
		SetSize(size2);
		memcpy(m_TabData, temp.m_TabData, temp.m_NbData * sizeof(DWORD));
	}

	// on effectue le OR bit à bit
	for(int i = 0; i < val.m_NbData; i++)
		m_TabData[i] |= val.m_TabData[i];

	// optimisation du résultat
	Optimize();
}


//***************************************************************************************
// XorBits : effectue un OU exclusif des bits.
// entrée : val : nombre avec lequel réaliser l'opération
//---------------------------------------------------------------------------------------
// Remarque : la taille du nombre est ajustée pour qu'il n'y est pas de dépassement.
//***************************************************************************************
void BigInt::XorBits(BigInt& val)
{
	// récupération des tailles
	int size1 = GetSize();
	int size2 = val.GetSize();

	// sauvegarde ancienne valeurs et modif de la taille
	if(size1 < size2)
	{
		BigInt temp = *this;
		SetSize(size2);
		memcpy(m_TabData, temp.m_TabData, temp.m_NbData * sizeof(DWORD));
	}

	// on effectue le XOR bit à bit
	for(int i = 0; i < val.m_NbData; i++)
		m_TabData[i] ^= val.m_TabData[i];

	// optimisation du résultat
	Optimize();
}


//***************************************************************************************
// Affectation en profondeur.
// entrée : val : nombre à recopier.
// retour : référence sur l'objet appelant (pour pouvoir faire a=b=c).
//***************************************************************************************
BigInt& BigInt::operator=(BigInt& val)
{
	// précaution en cas de a=a
	if(this == &val)
		return *this;

	// destruction de l'ancien tableau et copie des nouvelles données
	delete[] m_TabData;
	Copy(val);

	// retour
	return *this;
}

