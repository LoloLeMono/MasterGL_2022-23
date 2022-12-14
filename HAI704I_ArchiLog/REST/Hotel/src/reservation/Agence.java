package reservation;

import java.time.LocalDate;
import java.util.ArrayList;
// import java.util.Date;

public class Agence
{
	public String nom;
	public ArrayList<Hotel> li_HotelsPartenaire;
	
	public Agence(String nom, ArrayList<Hotel> li_HotelsPartenaire)
	{
		super();
		this.nom = nom;
		this.li_HotelsPartenaire = li_HotelsPartenaire;
	}

	public ArrayList<Chambre> recherche(String ville, LocalDate dateArr, LocalDate dateDep, int prixMin, int prixMax, int nbEtoiles, int nbLits)
	{
		ArrayList<Chambre> li_chambreResult = new ArrayList<Chambre>();
		
		for (Hotel h : li_HotelsPartenaire)
		{
			if (h.adresse.ville == ville & h.nbEtoiles == nbEtoiles & h.nbPlaces >= nbLits)
			{
				for (Chambre c : h.li_Chambres)
				{
					if (c.prix >= prixMin && c.prix <= prixMax && c.estLibre(dateArr, dateDep))
					{
						li_chambreResult.add(c);
					}
				}
			}
		}
		
		return li_chambreResult;
	}
}
