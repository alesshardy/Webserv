// Sélectionner toutes les sections
const sections = document.querySelectorAll('section');

// Ajouter des événements de survol
sections.forEach((section) => {
    section.addEventListener('mouseenter', () => {
        // Ajouter la classe "dimmed" à toutes les sections sauf celle survolée
        sections.forEach((s) => {
            if (s !== section) {
                s.classList.add('dimmed');
            }
        });
    });

    section.addEventListener('mouseleave', () => {
        // Supprimer la classe "dimmed" de toutes les sections
        sections.forEach((s) => {
            s.classList.remove('dimmed');
        });
    });
});