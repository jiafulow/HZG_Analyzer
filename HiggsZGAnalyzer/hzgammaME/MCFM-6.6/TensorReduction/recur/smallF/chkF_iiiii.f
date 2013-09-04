      subroutine chkF_iiiii(i1,i2,i3,i4,i5,m0sq,Gr,Czero5,N0)
C---  Expression for rearrangement of extension of Eq. 5.69
C---  Checks Diiii, requires D00iiiii
C---  Small terms of order Gr(i,j)*Dijklmno
C---  Denominator m0sq
      implicit none
      include 'constants.f'
      include 'Dnames.f'
      include 'Dv.f'
      include 'Darraydef.f'
      include 'Darrays.f'
      include 'weenumber.f' 
      integer ep,N0,i1,i2,i3,i4,i5,m,n,np
      parameter(np=3)
      double precision m0sq,Gr(np,np)
      double complex Czero5(z5max,-2:0),bit,pole,diff
       
      do ep=-2,0
      bit=czip
      do n=1,np
      do m=1,np
      bit=bit+Gr(n,m)*Dv(diiiiiii(z7(n,m,i1,i2,i3,i4,i5))+N0,ep)
      enddo
      enddo
      
      pole=czip
      if (ep .gt. -2) pole=4d0*Dv(dzziiiii(z5(i1,i2,i3,i4,i5))+N0,ep-1)
      
      diff=Dv(diiiii(z5(i1,i2,i3,i4,i5))+N0,ep)*2d0*m0sq-
     . (28d0*Dv(dzziiiii(z5(i1,i2,i3,i4,i5))+N0,ep)
     . -pole
     . -2d0*Czero5(z5(i1,i2,i3,i4,i5),ep)
     . +bit)

      if ((abs(diff) .gt. weenumber)) 
     . write(6,*) 'chkF_iiiii',i1,i2,i3,i4,i5,diff
     
      enddo
      
      return
      end
