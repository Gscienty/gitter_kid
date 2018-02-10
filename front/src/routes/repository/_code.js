import React, { Component } from 'react';
import { Card } from 'antd';
import ReactMarkdown from 'react-markdown';
import HighLight from 'react-syntax-highlight';


class CodeTemplate extends Component {
    state = { initialized: false, type: { name: 'md' } }

    extDict = {
        'cs': { name: 'csharp' },
        'xml': { name: 'xml' },
        'html': { name: 'html' },
        'json': { name: 'json' },
        'java': { name: 'java' },
        'md': { name: 'md' }
    }

    analysisContentType () {
        let defaultResult = { name: 'text' };
        if (this.props.name.indexOf ('.') === -1) {
            return defaultResult;
        }
        let ext = this.props.name.substring (this.props.name.lastIndexOf('.') + 1);

        return ext in this.extDict ? this.extDict[ext] : defaultResult;
        
    }

    componentWillMount () {
        this.props.dispatch({
            type: 'blob/getTextBlob',
            payload: {
                repositoriesName: this.props.repositoriesName,
                repositoryName: this.props.repositoryName,
                branchName: this.props.branchName,
                path: this.props.path
            },
            after: () => this.setState({ initialized: true, type: this.analysisContentType () })
        });
    }

    renderWaiting () {
        return <span>waiting...</span>;
    }

    renderContent () {
        return <Card
            title={ this.props.name }>
            {
                this.state.type.name === 'md'
                ? <ReactMarkdown source={ this.props.content } />
                : <HighLight lang={ this.state.type.name } value={ this.props.content } />
            }
        </Card>
    }

    render() {
        return this.state.initialized ? this.renderContent () : this.renderWaiting ();
    }
}

export default CodeTemplate;