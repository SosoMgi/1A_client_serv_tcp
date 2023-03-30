# Readme

## Createur
Sofian Mareghni et Thomas Triouleyre

## Usage

Pour compiler

```bash
    make
```

Pour clean les fichiers

```bash
    make
    make clean
```

Pour l'utilissation

```bash
    ./service_reseaux
```


## Documentation

    Dans cette version du tp, nous pouvons nous connecter au serveur utilisé du TP. Il faudras surement changer l'adresse ip si le fichier est testé sur une autre machine. La commande dir est bloquant au niveau du accepte. Sur wireshark, il y a marqué connection failed.
    Une fonction debug a été mis en place mais on a pas eu le temps de l'utilser au sein des programmes. Pour activer le mode debug, il faut taper "debug" et cela mettra une variable global deb à 1 qui lancera le mode debug.
    Les clients peuvent se deconnecter proprement en tapant ```/quit```.