# Analyse du Flux de Trafic par Vision par Ordinateur

## Introduction

Ce projet propose une solution innovante pour analyser les flux de trafic urbain en temps réel à l'aide de la vision par ordinateur et de l'intelligence artificielle. Il fournit des indicateurs clés de performance (KPIs) tels que le nombre de véhicules, leurs vitesses moyennes, leurs directions, et détecte les congestions de manière efficace et automatisée.

---

## Contexte et Motivation

### Contexte
La gestion traditionnelle du trafic, souvent coûteuse et rigide, repose sur des capteurs physiques et des méthodes manuelles limitées. Ce projet exploite les vidéos de surveillance avec des techniques avancées de vision par ordinateur pour offrir une solution scalable, flexible, et précise.

### Motivation
L'objectif est de répondre aux défis de la gestion du trafic urbain, tels que la congestion, les inefficacités, et les problèmes de sécurité, en utilisant une approche moderne et automatisée pour l'analyse des flux.

---

## Objectifs

1. Détecter et suivre les véhicules en temps réel à partir de flux vidéo.
2. Analyser les comportements des véhicules (vitesse, direction, etc.).
3. Fournir des statistiques exploitables pour l'optimisation des infrastructures et la mobilité.

---

## Technologies Utilisées

| Technologie       | Utilisation                                      |
|-------------------|--------------------------------------------------|
| **YOLOv3**        | Détection d'objets (véhicules et piétons).       |
| **OpenCV**        | Traitement des images et vidéos.                |
| **C++**           | Calcul des KPIs et communication backend.       |
| **Boost Asio**    | Gestion des WebSockets pour communication.       |
| **React/Next.js** | Développement du tableau de bord interactif.     |
| **nlohmann/json** | Structuration des données en JSON.              |

---

## Fonctionnalités Clés

### 1. Détection des Objets
- Utilisation de YOLOv3 via OpenCV pour détecter les véhicules et piétons.
- Précision améliorée grâce à l'entraînement personnalisé du modèle.

### 2. Calcul des Indicateurs Clés
- Nombre de véhicules (par type et région).
- Vitesse moyenne et temps moyen des véhicules.
- Détection des congestions et analyse des directions.

### 3. Tableau de Bord en Temps Réel
- Vidéo en direct avec annotations.
- Statistiques dynamiques sur les flux de trafic.
- Visualisation des KPIs (vitesse, temps, direction).

---

## Implémentation

This section explains the practical implementation of the traffic analysis system, including environment setup, model training, backend logic, and real-time communication with the frontend.

![mainImg](https://github.com/user-attachments/assets/ca2b7a41-d7e4-454c-8906-14b2432e3a70)
![vehicle_perf_metrics](https://github.com/user-attachments/assets/14380b4c-c37c-4ae8-b46c-ec2704adcf9d)
![traffic_stats](https://github.com/user-attachments/assets/8643028b-b6f7-4485-bfe5-2591576fe604)

---

### Entraînement du Modèle YOLOv3

Le modèle YOLOv3 a été ajusté pour détecter des véhicules spécifiques (voiture, bus, camion, moto). La commande suivante a été utilisée pour l'entraînement :

```bash
./darknet detector train obj.data yolov3.cfg yolov3.weights -dont_show -map
```

### Business Logic in C++

The backend is implemented in C++ using OpenCV and Boost libraries to process video streams and calculate real-time KPIs.

#### Video Processing and Object Detection

  - Reads each video frame.
  - Applies YOLOv3 for object detection.
  - Tracks detected objects and calculates movement direction.

#### KPIs Calculation

  - Vehicle Count by Region: Tracks the number of vehicles (cars, buses, trucks) in regions (North, South, East, West).
  - Average Speed and Time: Computes vehicle speeds and time spent in monitored zones.
  - Congestion Detection: Identifies areas with high vehicle density.

#### JSON Data Example Sent to Frontend
```json
{
  "region_stats": {
    "N": {"cars": 5, "buses": 2, "trucks": 1},
    "E": {"cars": 8, "buses": 1, "trucks": 0}
  },
  "average_speed": {"cars": 40, "buses": 30},
  "congestion_detected": true
}
```

### WebSocket Integration

  - Sends processed video frames (Base64-encoded) and KPI data (JSON) to the frontend.
  - Maintains real-time communication with clients using Boost Asio.

### Results Visualization

The data is streamed to a frontend application built with React/Next.js, providing:

  - Real-time traffic feed visualization.
  - Region-based vehicle statistics.
  - Key traffic metrics for improved decision-making.

## Conclusion

The implementation combines state-of-the-art vision and backend technologies to deliver a robust and scalable solution for traffic monitoring. The system supports real-time insights, enabling smarter traffic management and planning.







