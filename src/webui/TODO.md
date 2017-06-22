- [ ] Raccogliere lista TODO lasciati nel codice

- [ ] use case su dowse check DNS e search
- [x] togliere il .css.map
- [x] togliere il connection timeout con redis
- [ ] aggiungere la configurazione a run-time dei log
- [ ] aggiungere il pid nei log
- [ ] aggiungere in log-queue il parsing del pid
- [ ] unire log-channel e logqueue ?

- [x] modificare il database
- [x] la webui in kore_preload carica in redis la key del party_mode leggendo la variable="party_mode" dal db

- [x] nel dns plugin where_should.... se non c'e' la authorization controlla Redis
  - se c'e' il party_mode allora automaticamente passa e setta la key di authorization

- [x] nmap aggiunge la entry in found

- [x] quando si setta 'party mode'  a OFF vengono cancellate tutte le entry che non sono presenti nel DB




