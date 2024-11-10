Komunikimi me socket ndermjet klientit dhe serverit me TCP protokoll

Ky është një projekt për komunikimin ndërmjet serverit dhe klientit duke përdorur socket-e, zhvilluar në sistemin operativ Ubuntu. Serveri mbështet deri në 4 klientë dhe ofron funksionalitete si aktivizimi i privilegjeve të admin-it dhe ekzekutimi i komandave shell në server.

Funksionalitetet

Serveri:

Prit lidhjet nga klientët dhe menaxhon çdo klient në një thread të veçantë.

Mundëson privilegje admin për ekzekutimin e komandave shell.

Ruajtja e log-ëve të aktiviteteve dhe mesazheve të klientëve.

Mbyll lidhjet pas një periudhe inaktiviteti (timeout).

Klienti:

Lidhje me serverin përmes IP-së dhe portit të specifikuar.

Mund të aktivizoni privilegjet e admin-it dhe të ekzekutoni komanda shell te serveri.

Komunikon me serverin duke dërguar dhe marrë mesazhe.

Teknologjitë

C: Për krijimin e aplikacioneve server dhe klient.

Socket Programming: Komunikim ndërmjet serverit dhe klientit.

Pthreads: Përdorim i thread-ve për menaxhimin e klientëve të shumtë.

Ubuntu: Sistemi operativ në të cilin është zhvilluar dhe testuar ky projekt.

CLion: Mjet i zhvillimit të softuerit për programimin në C/C++.
