<?php
// Odbieramy dane JSON
$data = json_decode(file_get_contents('php://input'), true);

// Sprawdzamy, czy dane zostały poprawnie odebrane
if (isset($data['temperature'])) {
    $temperature = $data['temperature'];
    
    // Pobieramy aktualną datę i godzinę
    $date_time = date('Y-m-d H:i:s');  // Format: YYYY-MM-DD HH:MM:SS
    
    // Zapisujemy dane do pliku (log.txt) z datą i godziną
    $log = "Data: " . $date_time . " - Temperatura: " . $temperature . " °C\n";
    file_put_contents("log.txt", $log, FILE_APPEND);
    
    // Możesz wyświetlić te dane w odpowiedzi
    echo "Otrzymano temperaturę: " . $temperature . " °C - " . $date_time;
} else {
    // Jeśli nie udało się odebrać danych
    echo "Brak danych!";
}
?>
