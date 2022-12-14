package reservation;

import java.time.LocalDate;
import java.util.ArrayList;
// import java.util.Calendar;
// import java.util.Date;

public class Chambre
{
	public Integer nbLits;
	public Integer prix;
	public Hotel h;
	ArrayList<LocalDate> li_dateReserve;
	
	public Chambre(Integer nbLits, Integer prix, Hotel h)
	{
		super();
		this.nbLits = nbLits;
		this.prix = prix;
		this.h = h;
		this.li_dateReserve = new ArrayList<LocalDate>();
	}
	
	public Hotel getH()
	{
		return h;
	}

	public void setH(Hotel h)
	{
		this.h = h;
	}

	public Boolean estLibre(LocalDate dateArr, LocalDate dateDep)
	{
		ArrayList<LocalDate> li_dateClient = new ArrayList<LocalDate>();
		LocalDate dateBuff;
		Boolean trigg = true;
		
		dateBuff = dateArr;
		
		while(!dateBuff.equals(dateDep.plusDays(1)))
		{
			li_dateClient.add(dateBuff);
			dateBuff = dateBuff.plusDays(1);
		}
		
		
		for (int i = 0; i<this.li_dateReserve.size(); i++)
		{
			for (int j=0; j<li_dateClient.size(); j++)
			{
				if (li_dateReserve.get(i).equals(li_dateClient.get(j)))
				{
					trigg = false;
				}
			}
		}
		
		return trigg;
		
	}


	public Integer getNbLits()
	{
		return nbLits;
	}
	
	public void setNbLits(Integer nbLits)
	{
		this.nbLits = nbLits;
	}
	
	public ArrayList<LocalDate> getLi_dateReserve()
	{
		return li_dateReserve;
	}
	
	public void setLi_dateReserve(ArrayList<LocalDate> li)
	{
		this.li_dateReserve = li;
	}
	
	public Integer getPrix()
	{
		return prix;
	}
	
	public void setPrix(Integer prix)
	{
		this.prix = prix;
	}
	
	@Override
	public String toString()
	{
		return "Chambre de avec " + nbLits + " lits pour " + prix + "€ par nuit";
	}
	
	public Offre toOffre(int idOffre)
	{
		return new Offre(idOffre, this.li_dateReserve, this.nbLits, this.prix);
	}
}
