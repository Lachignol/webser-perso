// File upload

async function uploadFile() {
    const fileInput = document.getElementById('fileInput');
    const msg = document.getElementById('uploadMsg');
    msg.textContent = '';
    msg.style.color = '';
    if (!fileInput.files.length) {
        msg.textContent = "Select a file first.";
        msg.style.color = 'red';
        return;
    }
    const formData = new FormData();
    formData.append('file', fileInput.files[0]);
    try {
        const resp = await fetch('/upload/', { method: 'POST', body: formData });
        if (resp.ok) {
            msg.textContent = "✅ Successful upload.";
            msg.style.color = 'green';
            fileInput.value = '';
            displayUpload();
        } else {
            if (resp.status == 413) {
                msg.textContent = "❌ Upload failed: file is too large";
                msg.style.color = 'red';
            } else {
                msg.textContent = "❌ Upload failed.";
                msg.style.color = 'red';
            }
        }
    } catch (e) {
        msg.textContent = "Network error.";
        msg.style.color = 'red';
    }
}

async function displayUpload() {
    const listDiv = document.getElementById('uploadList');
    listDiv.innerHTML = '<em>Loading...</em>';
    try {
        const resp = await fetch('/display-upload/', { method: 'GET' });
        const html = await resp.text();
        const temp = document.createElement('div');
        temp.innerHTML = html;
        const ul = temp.querySelector('ul,ol');
        if (ul && ul.children.length > 0) {
            listDiv.innerHTML = '';
            Array.from(ul.children).forEach(li => {
                const a = li.querySelector('a');
                if (a) {
                    a.href = '/uploads/' + a.getAttribute('href');
                    a.textContent = a.textContent;
                    a.target = '_blank';
                }
                const filename = a ? a.textContent : li.textContent.split(/\s+/)[0];
                const btn = document.createElement('button');
                btn.textContent = 'Delete';
                btn.className = 'delete-btn';
                btn.style.marginLeft = '15px';
                btn.onclick = async () => {
                    await deleteFile(filename);
                };
                li.appendChild(btn);
            });
            listDiv.appendChild(ul);
        } else {
            listDiv.innerHTML = '<em>empty</em>';
        }
        updateSessionStats();
    } catch (e) {
        listDiv.innerHTML = '<em>Error</em>';
    }
}

async function deleteFile(filename) {
    const msg = document.getElementById('uploadMsg');
    msg.textContent = '';
    msg.style.color = '';
    try {
        const resp = await fetch('/delete-file/' + encodeURIComponent(filename), { method: 'DELETE' });
        if (resp.ok) {
            msg.textContent = "✅ File deleted.";
            msg.style.color = 'green';
            displayUpload();
        } else {
            msg.textContent = "❌ Deleted file failed.";
            msg.style.color = 'red';
        }
    } catch (e) {
        msg.textContent = "Network error.";
        msg.style.color = 'red';
    }
}

// Session stats

async function updateSessionStats() {
    try {
        const resp = await fetch('/api/session-stats');
        if (resp.ok) {
            const stats = await resp.json();
            updateCounter('pageViewsCount', stats.page_views || 0);
            updateCounter('uploadCount', stats.upload_count || 0);
            updateCounter('deleteCount', stats.delete_count || 0);
            updateCounter('cgiCount', stats.cgi_count || 0);
            
            updateElement('lastPageView', stats.last_page_view || 'never');
            updateElement('lastUpload', stats.last_upload || 'never');
            updateElement('lastDelete', stats.last_delete || 'never');
            updateElement('lastCgi', stats.last_cgi || 'never');
            
            updateElement('lastPageName', stats.last_page_name || '-');
            updateElement('lastUploadFile', stats.last_upload_file || '-');
            updateElement('lastDeleteFile', stats.last_delete_file || '-');
            updateElement('lastCgiScript', stats.last_cgi_script || '-');
        }
    } catch (e) {
        console.error('Error fetching session stats:', e);
    }
}

function updateCounter(elementId, newValue) {
    const element = document.getElementById(elementId);
    if (element) {
        element.textContent = newValue;
    }
}

function updateElement(elementId, newValue) {
    const element = document.getElementById(elementId);
    if (element) {
        element.textContent = newValue;
    }
}

// Rafraîchir la liste au chargement de la page
window.onload = async function() {
    await displayUpload();
    await updateSessionStats();
}
