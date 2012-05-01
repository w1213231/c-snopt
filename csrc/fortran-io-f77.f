      subroutine fopenwrap
     &   (info, fd, fname, status)
      implicit none
      integer fd, status, info
      character*(*) fname

      if (status .eq. 0) then
         open(fd, iostat=info, file=fname, status='OLD')
      else if (status .eq. 1) then
         open(fd, iostat=info, file=fname, status='NEW')
      else if (status .eq. 2) then
         open(fd, iostat=info, file=fname, status='SCRATCH')
      else if (status .eq. 3) then
         open(fd, iostat=info, file=fname, status='REPLACE')
      else if (status .eq. 4) then
         open(fd, iostat=info, file=fname, status='UNKNOWN')
      else if (status .eq. 5) then
         open(fd, iostat=info, file=fname, status='APPEND')
      end if
      
      end


      subroutine fclosewrap
     & (fd)
      implicit none

      integer fd

      close(fd)

      end
