<?php
// Odbieramy dane JSON
$data = json_decode(file_get_contents('php://input'), true);

// Sprawdzamy, czy dane zostały poprawnie odebrane
if (isset($data['temperature'])) {
    $temperature = $data['temperature'];
    
    // Zapisujemy dane do pliku (log.txt)
    $log = "Temperatura: " . $temperature . " °C\n";
    file_put_contents("log.txt", $log, FILE_APPEND);
    
    // Możesz wyświetlić te dane w odpowiedzi
    echo "Otrzymano temperaturę: " . $temperature . " °C";
} else {
    // Jeśli nie udało się odebrać danych
    echo "Brak danychh!";
}
?>
